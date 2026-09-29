#include "6g-access-node.h"
#include "6g-edge-node.h"

#include <algorithm>

namespace ns3
{

NS_OBJECT_ENSURE_REGISTERED(SixGAccessNode);

TypeId
SixGAccessNode::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::SixGAccessNode")
            .SetParent<SixGNode>()
            .SetGroupName("6g-lena")
            .AddConstructor<SixGAccessNode>();

    return tid;
}

SixGAccessNode::SixGAccessNode()
    : m_connectedUes(),
      m_communicationCapacity(0.0),
      m_availableCommunication(0.0),
      m_coverageRadius(0.0),
      m_load(0.0)
{
    Ptr<SixGResource> communicationResource = CreateObject<SixGResource>();

    communicationResource->SetType("COMMUNICATION");
    communicationResource->SetCapacity(0.0);
    communicationResource->SetAvailable(0.0);

    AddResource(communicationResource);
}

SixGAccessNode::~SixGAccessNode() = default;

void
SixGAccessNode::AddConnectedUe(const std::string& ueId)
{
    if (!HasConnectedUe(ueId))
    {
        m_connectedUes.push_back(ueId);
    }
}

void
SixGAccessNode::RemoveConnectedUe(const std::string& ueId)
{
    auto it = std::find(m_connectedUes.begin(), m_connectedUes.end(), ueId);

    if (it != m_connectedUes.end())
    {
        m_connectedUes.erase(it);
    }
}

uint32_t
SixGAccessNode::GetConnectedUeCount() const
{
    return static_cast<uint32_t>(m_connectedUes.size());
}

bool
SixGAccessNode::HasConnectedUe(const std::string& ueId) const
{
    return std::find(m_connectedUes.begin(), m_connectedUes.end(), ueId) !=
           m_connectedUes.end();
}

void
SixGAccessNode::AddReachableEdgeNode(Ptr<SixGEdgeNode> edgeNode)
{
    if (!edgeNode || HasReachableEdgeNode(edgeNode))
    {
        return;
    }

    m_reachableEdgeNodes.push_back(edgeNode);
}

void
SixGAccessNode::RemoveReachableEdgeNode(Ptr<SixGEdgeNode> edgeNode)
{
    auto it = std::find(m_reachableEdgeNodes.begin(),
                        m_reachableEdgeNodes.end(),
                        edgeNode);

    if (it != m_reachableEdgeNodes.end())
    {
        m_reachableEdgeNodes.erase(it);
    }
}

bool
SixGAccessNode::HasReachableEdgeNode(Ptr<SixGEdgeNode> edgeNode) const
{
    return std::find(m_reachableEdgeNodes.begin(),
                     m_reachableEdgeNodes.end(),
                     edgeNode) != m_reachableEdgeNodes.end();
}

uint32_t
SixGAccessNode::GetReachableEdgeNodeCount() const
{
    return static_cast<uint32_t>(m_reachableEdgeNodes.size());
}

Ptr<SixGEdgeNode>
SixGAccessNode::GetReachableEdgeNode(uint32_t index) const
{
    if (index >= m_reachableEdgeNodes.size())
    {
        return nullptr;
    }

    return m_reachableEdgeNodes[index];
}

void
SixGAccessNode::SetCommunicationCapacity(double capacity)
{
    if (capacity < 0.0)
    {
        return;
    }

    Ptr<SixGResource> resource = GetResource("COMMUNICATION");

    if (!resource)
    {
        return;
    }

    if (!resource->SetCapacity(capacity))
    {
        return;
    }

    m_communicationCapacity = capacity;

    if (resource->GetAvailable() > capacity)
    {
        resource->SetAvailable(capacity);
    }

    m_availableCommunication = resource->GetAvailable();
}

double
SixGAccessNode::GetCommunicationCapacity() const
{
    Ptr<SixGResource> resource = GetResource("COMMUNICATION");

    if (resource)
    {
        return resource->GetCapacity();
    }

    return m_communicationCapacity;
}

void
SixGAccessNode::SetAvailableCommunication(double available)
{
    Ptr<SixGResource> resource = GetResource("COMMUNICATION");

    if (!resource)
    {
        return;
    }

    if (!resource->SetAvailable(available))
    {
        return;
    }

    m_availableCommunication = resource->GetAvailable();
}

double
SixGAccessNode::GetAvailableCommunication() const
{
    Ptr<SixGResource> resource = GetResource("COMMUNICATION");

    if (resource)
    {
        return resource->GetAvailable();
    }

    return m_availableCommunication;
}

void
SixGAccessNode::SetCoverageRadius(double radius)
{
    m_coverageRadius = radius;
}

double
SixGAccessNode::GetCoverageRadius() const
{
    return m_coverageRadius;
}

void
SixGAccessNode::SetLoad(double load)
{
    m_load = load;
}

double
SixGAccessNode::GetLoad() const
{
    return m_load;
}

} // namespace ns3
