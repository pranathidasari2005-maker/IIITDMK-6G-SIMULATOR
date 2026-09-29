#include "6g-capability-registry.h"

#include "ns3/log.h"
#include "ns3/type-id.h"

#include <algorithm>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("SixGCapabilityRegistry");

NS_OBJECT_ENSURE_REGISTERED(SixGCapabilityRegistry);

TypeId
SixGCapabilityRegistry::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::SixGCapabilityRegistry")
            .SetParent<Object>()
            .SetGroupName("6g-capabilities")
            .AddConstructor<SixGCapabilityRegistry>();

    return tid;
}

SixGCapabilityRegistry::SixGCapabilityRegistry() = default;

SixGCapabilityRegistry::~SixGCapabilityRegistry()
{
    m_registrations.clear();
}

void
SixGCapabilityRegistry::RegisterCapability(
    Ptr<Node> node,
    Ptr<SixGCapability> capability)
{
    if (node == nullptr || capability == nullptr)
    {
        NS_LOG_WARN("Cannot register a null node or capability.");
        return;
    }

    CapabilityRegistration registration;
    registration.node = node;
    registration.capability = capability;

    m_registrations.push_back(registration);

    NS_LOG_INFO("Registered capability '"
                << capability->GetCapabilityName()
                << "' for node "
                << node->GetId());
}

bool
SixGCapabilityRegistry::RemoveCapability(
    Ptr<Node> node,
    Ptr<SixGCapability> capability)
{
    if (node == nullptr || capability == nullptr)
    {
        return false;
    }

    auto it = std::find_if(
        m_registrations.begin(),
        m_registrations.end(),
        [node, capability](const CapabilityRegistration& registration)
        {
            return registration.node == node &&
                   registration.capability == capability;
        });

    if (it == m_registrations.end())
    {
        return false;
    }

    m_registrations.erase(it);

    NS_LOG_INFO("Removed capability '"
                << capability->GetCapabilityName()
                << "' from node "
                << node->GetId());

    return true;
}

std::vector<SixGCapabilityRegistry::CapabilityRegistration>
SixGCapabilityRegistry::FindCapabilitiesByType(
    SixGCapability::CapabilityType type) const
{
    std::vector<CapabilityRegistration> result;

    for (const auto& registration : m_registrations)
    {
        if (registration.capability != nullptr &&
            registration.capability->GetCapabilityType() == type)
        {
            result.push_back(registration);
        }
    }

    return result;
}

std::vector<SixGCapabilityRegistry::CapabilityRegistration>
SixGCapabilityRegistry::GetRegistrations() const
{
    return m_registrations;
}

uint32_t
SixGCapabilityRegistry::GetCapabilityCount() const
{
    return static_cast<uint32_t>(m_registrations.size());
}

} // namespace ns3
