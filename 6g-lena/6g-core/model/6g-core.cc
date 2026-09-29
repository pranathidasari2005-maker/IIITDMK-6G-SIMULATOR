#include "6g-core.h"

#include "ns3/log.h"
#include "ns3/simulator.h"

#include <algorithm>
#include <cerrno>
#include <cstdlib>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("SixGCore");

NS_OBJECT_ENSURE_REGISTERED(SixGCore);

namespace
{

bool
IsNumeric(const std::string& value, double& number)
{
    if (value.empty())
    {
        return false;
    }

    char* end = nullptr;
    errno = 0;

    const double parsed = std::strtod(value.c_str(), &end);

    if (errno != 0 || end == value.c_str() || *end != '\0')
    {
        return false;
    }

    number = parsed;
    return true;
}

bool
EvaluateConstraint(const SixGCapability& capability,
                   const SixGCapabilityConstraint& constraint)
{
    const auto attribute = capability.attributes.find(constraint.attribute);

    if (attribute == capability.attributes.end())
    {
        return false;
    }

    const std::string& actual = attribute->second;
    const std::string& required = constraint.value;

    switch (constraint.op)
    {
    case SixGConstraintOperator::EQUAL:
        return actual == required;

    case SixGConstraintOperator::NOT_EQUAL:
        return actual != required;

    case SixGConstraintOperator::GREATER_THAN:
    case SixGConstraintOperator::GREATER_EQUAL:
    case SixGConstraintOperator::LESS_THAN:
    case SixGConstraintOperator::LESS_EQUAL:
    {
        double actualValue = 0.0;
        double requiredValue = 0.0;

        if (!IsNumeric(actual, actualValue) ||
            !IsNumeric(required, requiredValue))
        {
            return false;
        }

        switch (constraint.op)
        {
        case SixGConstraintOperator::GREATER_THAN:
            return actualValue > requiredValue;

        case SixGConstraintOperator::GREATER_EQUAL:
            return actualValue >= requiredValue;

        case SixGConstraintOperator::LESS_THAN:
            return actualValue < requiredValue;

        case SixGConstraintOperator::LESS_EQUAL:
            return actualValue <= requiredValue;

        default:
            return false;
        }
    }
    }

    return false;
}

} // namespace

TypeId
SixGCore::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::SixGCore")
            .SetParent<Object>()
            .SetGroupName("6g-core")
            .AddConstructor<SixGCore>();
    return tid;
}

void
SixGCore::SetScenario(const std::string& scenario)
{
    m_scenario = scenario;
}

std::string
SixGCore::GetScenario() const
{
    return m_scenario;
}

void
SixGCore::Initialize()
{
    NS_LOG_INFO("Initializing 6G-LENA core framework");
    NS_LOG_INFO("Scenario: " << m_scenario);

    m_initialized = true;
}

bool
SixGCore::IsInitialized() const
{
    return m_initialized;
}

bool
SixGCore::RegisterCapability(const SixGCapability& capability)
{
    if (capability.nodeId.empty() || capability.type.empty())
    {
        return false;
    }

    const auto existing =
        std::find_if(m_capabilities.begin(),
                     m_capabilities.end(),
                     [&capability](const SixGCapability& registered)
                     {
                         return registered.nodeId == capability.nodeId &&
                                registered.type == capability.type;
                     });

    if (existing != m_capabilities.end())
    {
        return false;
    }

    SixGCapability registeredCapability = capability;
    registeredCapability.lastUpdated = Simulator::Now();

    m_capabilities.push_back(registeredCapability);

    NS_LOG_INFO("Registered capability: node="
                << capability.nodeId
                << ", type="
                << capability.type
                << ", capacity="
                << capability.capacity
                << ", availableResource="
                << capability.availableResource);

    return true;
}

bool
SixGCore::UpdateCapability(const SixGCapability& capability)
{
    if (capability.nodeId.empty() || capability.type.empty())
    {
        return false;
    }

    const auto existing =
        std::find_if(m_capabilities.begin(),
                     m_capabilities.end(),
                     [&capability](const SixGCapability& registered)
                     {
                         return registered.nodeId == capability.nodeId &&
                                registered.type == capability.type;
                     });

    if (existing == m_capabilities.end())
    {
        return false;
    }

    SixGCapability updatedCapability = capability;
    updatedCapability.lastUpdated = Simulator::Now();

    *existing = updatedCapability;

    NS_LOG_INFO("Updated capability: node="
                << capability.nodeId
                << ", type="
                << capability.type
                << ", capacity="
                << capability.capacity
                << ", availableResource="
                << capability.availableResource
                << ", available="
                << capability.available);

    return true;
}

bool
SixGCore::HasCapability(const std::string& nodeId,
                        const std::string& type) const
{
    const auto existing =
        std::find_if(m_capabilities.begin(),
                     m_capabilities.end(),
                     [&nodeId, &type](const SixGCapability& capability)
                     {
                         return capability.nodeId == nodeId &&
                                capability.type == type;
                     });

    return existing != m_capabilities.end();
}

std::vector<SixGCapability>
SixGCore::GetCapabilities() const
{
    return m_capabilities;
}

bool
SixGCore::RemoveCapability(const std::string& nodeId,
                           const std::string& type)
{
    const auto oldSize = m_capabilities.size();

    m_capabilities.erase(
        std::remove_if(m_capabilities.begin(),
                       m_capabilities.end(),
                       [&nodeId, &type](const SixGCapability& capability)
                       {
                           return capability.nodeId == nodeId &&
                                  capability.type == type;
                       }),
        m_capabilities.end());

    return m_capabilities.size() != oldSize;
}

bool
SixGCore::IsCapabilityFresh(const SixGCapability& capability) const
{
    // A zero validity duration means freshness checking is disabled.
    if (capability.validityDuration == Seconds(0))
    {
        return true;
    }

    const Time age = Simulator::Now() - capability.lastUpdated;

    return age <= capability.validityDuration;
}

std::vector<SixGCapability>
SixGCore::DiscoverCapabilities(const SixGCapabilityRequest& request) const
{
    std::vector<SixGCapability> matches;

    for (const auto& capability : m_capabilities)
    {
        if (capability.type != request.type)
        {
            continue;
        }

        if (capability.availableResource < request.minimumResource)
        {
            continue;
        }

        if (request.requireAvailable &&
            (!capability.available ||
             capability.state == SixGCapabilityState::UNAVAILABLE))
        {
            continue;
        }

        if (request.requireFresh && !IsCapabilityFresh(capability))
        {
            continue;
        }

        bool attributesMatch = true;

        for (const auto& required : request.requiredAttributes)
        {
            const auto attribute = capability.attributes.find(required.first);

            if (attribute == capability.attributes.end() ||
                attribute->second != required.second)
            {
                attributesMatch = false;
                break;
            }
        }

        if (!attributesMatch)
        {
            continue;
        }

        bool constraintsMatch = true;

        for (const auto& constraint : request.constraints)
        {
            if (!EvaluateConstraint(capability, constraint))
            {
                constraintsMatch = false;
                break;
            }
        }

        if (!constraintsMatch)
        {
            continue;
        }

        matches.push_back(capability);
    }

    NS_LOG_INFO("Capability discovery request: type="
                << request.type
                << ", minimum resource="
                << request.minimumResource
                << ", required attributes="
                << request.requiredAttributes.size()
                << ", matches="
                << matches.size());

    return matches;
}

std::vector<SixGCapability>
SixGCore::DiscoverCapabilities(
    const SixGCapabilityRequest& request,
    const std::vector<std::string>& candidateNodeIds) const
{
    std::vector<SixGCapability> matches;

    for (const auto& capability : m_capabilities)
    {
        if (std::find(candidateNodeIds.begin(),
                      candidateNodeIds.end(),
                      capability.nodeId) == candidateNodeIds.end())
        {
            continue;
        }

        if (!request.type.empty() && capability.type != request.type)
        {
            continue;
        }

        if (capability.availableResource < request.minimumResource)
        {
            continue;
        }

        if (request.requireAvailable &&
            (!capability.available ||
             capability.state == SixGCapabilityState::UNAVAILABLE))
        {
            continue;
        }

        if (request.requireFresh && !IsCapabilityFresh(capability))
        {
            continue;
        }

        bool attributesMatch = true;

        for (const auto& required : request.requiredAttributes)
        {
            auto it = capability.attributes.find(required.first);

            if (it == capability.attributes.end() ||
                it->second != required.second)
            {
                attributesMatch = false;
                break;
            }
        }

        if (!attributesMatch)
        {
            continue;
        }

        bool constraintsMatch = true;

        for (const auto& constraint : request.constraints)
        {
            if (!EvaluateConstraint(capability, constraint))
            {
                constraintsMatch = false;
                break;
            }
        }

        if (!constraintsMatch)
        {
            continue;
        }

        matches.push_back(capability);
    }

    return matches;
}


} // namespace ns3
