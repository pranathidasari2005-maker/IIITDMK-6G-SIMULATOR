#include "network-nodes.h"

#include "ns3/fatal-error.h"
#include "ns3/log.h"

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
            .SetGroupName("network-nodes")
            .AddConstructor<SixGNode>();

    return tid;
}

SixGNode::SixGNode()
    : m_initialized(false)
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
SixGNode::AddCapability(Ptr<SixGCapability> capability)
{
    if (capability)
    {
        m_capabilities.push_back(capability);
    }
}

uint32_t
SixGNode::GetCapabilityCount() const
{
    return m_capabilities.size();
}

Ptr<SixGCapability>
SixGNode::GetCapability(uint32_t index) const
{
    NS_ABORT_MSG_IF(index >= m_capabilities.size(),
                    "Capability index out of range");

    return m_capabilities[index];
}

} // namespace ns3
