#include "6g-edge-node.h"

#include "ns3/log.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("SixGEdgeNode");

NS_OBJECT_ENSURE_REGISTERED(SixGEdgeNode);

TypeId
SixGEdgeNode::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::SixGEdgeNode")
            .SetParent<SixGNode>()
            .SetGroupName("6g-lena")
            .AddConstructor<SixGEdgeNode>();
    return tid;
}

SixGEdgeNode::SixGEdgeNode()
    : m_connectedCoreId(""), m_computeCapacity(0.0),
      m_availableCompute(0.0),
      m_memoryCapacity(0.0),
      m_availableMemory(0.0),
      m_applicationLoad(0.0)
{
    Ptr<SixGResource> computeResource = CreateObject<SixGResource>();

    computeResource->SetType("COMPUTING");
    computeResource->SetCapacity(0.0);
    computeResource->SetAvailable(0.0);

    AddResource(computeResource);

    Ptr<SixGResource> memoryResource = CreateObject<SixGResource>();

    memoryResource->SetType("MEMORY");
    memoryResource->SetCapacity(0.0);
    memoryResource->SetAvailable(0.0);

    AddResource(memoryResource);
}

SixGEdgeNode::~SixGEdgeNode() = default;

void
SixGEdgeNode::SetConnectedCoreId(const std::string& coreId)
{
    m_connectedCoreId = coreId;
}

std::string
SixGEdgeNode::GetConnectedCoreId() const
{
    return m_connectedCoreId;
}

void
SixGEdgeNode::SetComputeCapacity(double capacity)
{
    if (capacity < 0.0)
    {
        return;
    }

    Ptr<SixGResource> resource = GetResource("COMPUTING");

    if (!resource)
    {
        return;
    }

    if (!resource->SetCapacity(capacity))
    {
        return;
    }

    m_computeCapacity = capacity;

    if (resource->GetAvailable() > capacity)
    {
        resource->SetAvailable(capacity);
    }

    m_availableCompute = resource->GetAvailable();
}

double
SixGEdgeNode::GetComputeCapacity() const
{
    Ptr<SixGResource> resource = GetResource("COMPUTING");

    if (resource)
    {
        return resource->GetCapacity();
    }

    return m_computeCapacity;
}

void
SixGEdgeNode::SetAvailableCompute(double available)
{
    Ptr<SixGResource> resource = GetResource("COMPUTING");

    if (!resource)
    {
        return;
    }

    if (!resource->SetAvailable(available))
    {
        return;
    }

    m_availableCompute = resource->GetAvailable();
}

double
SixGEdgeNode::GetAvailableCompute() const
{
    Ptr<SixGResource> resource = GetResource("COMPUTING");

    if (resource)
    {
        return resource->GetAvailable();
    }

    return m_availableCompute;
}

bool
SixGEdgeNode::ConsumeCompute(double amount)
{
    Ptr<SixGResource> resource = GetResource("COMPUTING");

    if (!resource)
    {
        return false;
    }

    if (!resource->Consume(amount))
    {
        return false;
    }

    m_availableCompute = resource->GetAvailable();

    return true;
}

bool
SixGEdgeNode::ReleaseCompute(double amount)
{
    Ptr<SixGResource> resource = GetResource("COMPUTING");

    if (!resource)
    {
        return false;
    }

    if (!resource->Release(amount))
    {
        return false;
    }

    m_availableCompute = resource->GetAvailable();

    return true;
}

void
SixGEdgeNode::SetMemoryCapacity(double capacity)
{
    if (capacity < 0.0)
    {
        return;
    }

    Ptr<SixGResource> resource = GetResource("MEMORY");

    if (!resource)
    {
        return;
    }

    if (!resource->SetCapacity(capacity))
    {
        return;
    }

    m_memoryCapacity = capacity;

    if (resource->GetAvailable() > capacity)
    {
        resource->SetAvailable(capacity);
    }

    m_availableMemory = resource->GetAvailable();
}

double
SixGEdgeNode::GetMemoryCapacity() const
{
    Ptr<SixGResource> resource = GetResource("MEMORY");

    if (resource)
    {
        return resource->GetCapacity();
    }

    return m_memoryCapacity;
}

void
SixGEdgeNode::SetAvailableMemory(double available)
{
    Ptr<SixGResource> resource = GetResource("MEMORY");

    if (!resource)
    {
        return;
    }

    if (!resource->SetAvailable(available))
    {
        return;
    }

    m_availableMemory = resource->GetAvailable();
}

double
SixGEdgeNode::GetAvailableMemory() const
{
    Ptr<SixGResource> resource = GetResource("MEMORY");

    if (resource)
    {
        return resource->GetAvailable();
    }

    return m_availableMemory;
}

void
SixGEdgeNode::SetApplicationLoad(double load)
{
    m_applicationLoad = load;
}

double
SixGEdgeNode::GetApplicationLoad() const
{
    return m_applicationLoad;
}

} // namespace ns3
