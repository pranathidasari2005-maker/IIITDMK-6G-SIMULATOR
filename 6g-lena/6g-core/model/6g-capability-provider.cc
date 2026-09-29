#include "6g-capability-provider.h"

#include "ns3/log.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("SixGCapabilityProvider");

NS_OBJECT_ENSURE_REGISTERED(SixGCapabilityProvider);

TypeId
SixGCapabilityProvider::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::SixGCapabilityProvider")
            .SetParent<Object>()
            .SetGroupName("6g-core")
            .AddConstructor<SixGCapabilityProvider>();

    return tid;
}

void
SixGCapabilityProvider::SetCore(Ptr<SixGCore> core)
{
    m_core = core;
}

void
SixGCapabilityProvider::SetStateMonitor(Ptr<SixGStateMonitor> monitor)
{
    m_stateMonitor = monitor;
}

void
SixGCapabilityProvider::SetResource(Ptr<SixGResource> resource)
{
    if (!resource)
    {
        return;
    }

    m_resource = resource;

    if (m_capability.type.empty())
    {
        m_capability.type = resource->GetType();
    }

    m_capability.capacity = resource->GetCapacity();
    m_capability.availableResource = resource->GetAvailable();
}

Ptr<SixGResource>
SixGCapabilityProvider::GetResource() const
{
    return m_resource;
}

bool
SixGCapabilityProvider::UpdateFromState()
{
    if (m_capability.type.empty())
    {
        return false;
    }

    /*
     * Resource quantities come from the generic SixGResource.
     * This makes the resource object the source of truth for
     * capability capacity and availability.
     */
    if (m_resource)
    {
        if (m_resource->GetType().empty())
        {
            return false;
        }

        if (m_capability.type != m_resource->GetType())
        {
            return false;
        }

        m_capability.capacity = m_resource->GetCapacity();
        m_capability.availableResource = m_resource->GetAvailable();
    }
    else if (m_stateMonitor)
    {
        if (m_capability.type == "COMPUTING")
        {
            m_capability.capacity = m_stateMonitor->GetComputeCapacity();
            m_capability.availableResource = m_stateMonitor->GetAvailableCompute();
        }
        else if (m_capability.type == "COMMUNICATION")
        {
            m_capability.capacity = m_stateMonitor->GetCommunicationCapacity();
            m_capability.availableResource =
                m_stateMonitor->GetAvailableCommunication();
        }
    }

    /*
     * StateMonitor remains responsible for node-level dynamic state
     * such as node identity, mobility and connectivity.
     */
    if (m_stateMonitor)
    {
        if (m_stateMonitor->GetNodeId().empty())
        {
            return false;
        }

        if (m_stateMonitor->UpdateFromMobility())
        {
            m_capability.attributes["x"] =
                std::to_string(m_stateMonitor->GetPosition().x);
            m_capability.attributes["y"] =
                std::to_string(m_stateMonitor->GetPosition().y);
            m_capability.attributes["z"] =
                std::to_string(m_stateMonitor->GetPosition().z);
        }

        m_capability.nodeId = m_stateMonitor->GetNodeId();
        m_capability.available = m_stateMonitor->IsConnected();
    }

    if (m_core)
    {
        if (m_core->HasCapability(
                m_capability.nodeId,
                m_capability.type))
        {
            return m_core->UpdateCapability(m_capability);
        }

        return m_core->RegisterCapability(m_capability);
    }

    return true;
}

void
SixGCapabilityProvider::SetNodeId(const std::string& nodeId)
{
    m_capability.nodeId = nodeId;
}

void
SixGCapabilityProvider::SetCapabilityType(const std::string& type)
{
    m_capability.type = type;

    if (m_resource && m_resource->GetType() != type)
    {
        NS_LOG_WARN("Capability type does not match attached resource type");
    }
}

void
SixGCapabilityProvider::SetCapacity(double capacity)
{
    if (capacity < 0.0)
    {
        return;
    }

    if (m_resource)
    {
        if (!m_resource->SetCapacity(capacity))
        {
            return;
        }

        m_capability.capacity = m_resource->GetCapacity();
        m_capability.availableResource = m_resource->GetAvailable();
    }
    else
    {
        m_capability.capacity = capacity;
        m_capability.availableResource = capacity;
    }

    if (m_core && m_core->HasCapability(m_capability.nodeId, m_capability.type))
    {
        m_core->UpdateCapability(m_capability);
    }
}

void
SixGCapabilityProvider::SetAvailableResource(double availableResource)
{
    if (availableResource < 0.0)
    {
        return;
    }

    if (m_resource)
    {
        if (!m_resource->SetAvailable(availableResource))
        {
            return;
        }

        m_capability.capacity = m_resource->GetCapacity();
        m_capability.availableResource = m_resource->GetAvailable();
    }
    else
    {
        if (availableResource > m_capability.capacity)
        {
            return;
        }

        m_capability.availableResource = availableResource;
    }

    if (m_core && m_core->HasCapability(m_capability.nodeId, m_capability.type))
    {
        m_core->UpdateCapability(m_capability);
    }
}

void
SixGCapabilityProvider::SetAvailable(bool available)
{
    m_capability.available = available;

    if (m_core && m_core->HasCapability(m_capability.nodeId, m_capability.type))
    {
        m_core->UpdateCapability(m_capability);
    }
}

void
SixGCapabilityProvider::SetState(SixGCapabilityState state)
{
    m_capability.state = state;

    if (m_core && m_core->HasCapability(m_capability.nodeId, m_capability.type))
    {
        m_core->UpdateCapability(m_capability);
    }
}

SixGCapabilityState
SixGCapabilityProvider::GetState() const
{
    return m_capability.state;
}

void
SixGCapabilityProvider::SetValidityDuration(Time duration)
{
    if (duration < Seconds(0))
    {
        return;
    }

    m_capability.validityDuration = duration;

    if (m_core && m_core->HasCapability(m_capability.nodeId, m_capability.type))
    {
        m_core->UpdateCapability(m_capability);
    }
}

void
SixGCapabilityProvider::SetAttribute(const std::string& name,
                                     const std::string& value)
{
    if (name.empty())
    {
        return;
    }

    m_capability.attributes[name] = value;

    if (m_core && m_core->HasCapability(m_capability.nodeId, m_capability.type))
    {
        m_core->UpdateCapability(m_capability);
    }
}

std::string
SixGCapabilityProvider::GetAttribute(const std::string& name) const
{
    const auto it = m_capability.attributes.find(name);

    if (it == m_capability.attributes.end())
    {
        return "";
    }

    return it->second;
}

bool
SixGCapabilityProvider::Register()
{
    if (!m_core)
    {
        NS_LOG_ERROR("Cannot register capability: SixGCore is not set");
        return false;
    }

    return m_core->RegisterCapability(m_capability);
}

bool
SixGCapabilityProvider::ConsumeResource(double amount)
{
    if (amount < 0.0)
    {
        return false;
    }

    if (m_resource)
    {
        if (!m_resource->Consume(amount))
        {
            return false;
        }

        m_capability.capacity = m_resource->GetCapacity();
        m_capability.availableResource = m_resource->GetAvailable();

        return UpdateFromState();
    }

    if (amount > m_capability.availableResource)
    {
        return false;
    }

    m_capability.availableResource -= amount;

    if (!m_core)
    {
        return true;
    }

    if (!m_core->HasCapability(m_capability.nodeId, m_capability.type))
    {
        return false;
    }

    return m_core->UpdateCapability(m_capability);
}

bool
SixGCapabilityProvider::ReleaseResource(double amount)
{
    if (amount < 0.0)
    {
        return false;
    }

    if (m_resource)
    {
        if (!m_resource->Release(amount))
        {
            return false;
        }

        m_capability.capacity = m_resource->GetCapacity();
        m_capability.availableResource = m_resource->GetAvailable();

        return UpdateFromState();
    }

    if (m_capability.availableResource + amount > m_capability.capacity)
    {
        return false;
    }

    m_capability.availableResource += amount;

    if (!m_core)
    {
        return true;
    }

    if (!m_core->HasCapability(m_capability.nodeId, m_capability.type))
    {
        return false;
    }

    return m_core->UpdateCapability(m_capability);
}

SixGCapability
SixGCapabilityProvider::GetCapability() const
{
    return m_capability;
}

} // namespace ns3
