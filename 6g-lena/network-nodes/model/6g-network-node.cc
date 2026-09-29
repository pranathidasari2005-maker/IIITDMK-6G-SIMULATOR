#include "6g-network-node.h"

#include "ns3/log.h"

#include <algorithm>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("SixGNode");

NS_OBJECT_ENSURE_REGISTERED(SixGNode);

TypeId
SixGNode::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::SixGNode")
            .SetParent<Object>()
            .SetGroupName("6g-lena")
            .AddConstructor<SixGNode>();

    return tid;
}

SixGNode::SixGNode()
    : m_initialized(false),
      m_nodeId(""),
      m_position(Vector(0.0, 0.0, 0.0)),
      m_active(true),
      m_resources()
{
}

SixGNode::~SixGNode() = default;

void
SixGNode::Initialize()
{
    m_initialized = true;
}

bool
SixGNode::IsInitialized() const
{
    return m_initialized;
}

void
SixGNode::SetNodeId(const std::string& nodeId)
{
    m_nodeId = nodeId;
}

std::string
SixGNode::GetNodeId() const
{
    return m_nodeId;
}

void
SixGNode::SetPosition(const Vector& position)
{
    m_position = position;
}

Vector
SixGNode::GetPosition() const
{
    return m_position;
}

void
SixGNode::SetActive(bool active)
{
    m_active = active;
}

bool
SixGNode::IsActive() const
{
    return m_active;
}

bool
SixGNode::AddResource(Ptr<SixGResource> resource)
{
    if (resource == nullptr || resource->GetType().empty())
    {
        return false;
    }

    if (HasResource(resource->GetType()))
    {
        return false;
    }

    m_resources.push_back(resource);
    return true;
}

Ptr<SixGResource>
SixGNode::GetResource(const std::string& type) const
{
    for (const auto& resource : m_resources)
    {
        if (resource != nullptr && resource->GetType() == type)
        {
            return resource;
        }
    }

    return nullptr;
}

bool
SixGNode::HasResource(const std::string& type) const
{
    return GetResource(type) != nullptr;
}

bool
SixGNode::RemoveResource(const std::string& type)
{
    auto it = std::find_if(
        m_resources.begin(),
        m_resources.end(),
        [&type](const Ptr<SixGResource>& resource)
        {
            return resource != nullptr && resource->GetType() == type;
        });

    if (it == m_resources.end())
    {
        return false;
    }

    m_resources.erase(it);
    return true;
}

const std::vector<Ptr<SixGResource>>&
SixGNode::GetResources() const
{
    return m_resources;
}

} // namespace ns3
