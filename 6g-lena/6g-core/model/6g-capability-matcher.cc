#include "6g-capability-matcher.h"

#include "ns3/log.h"

#include <algorithm>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("SixGCapabilityMatcher");

NS_OBJECT_ENSURE_REGISTERED(SixGCapabilityMatcher);

TypeId
SixGCapabilityMatcher::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::SixGCapabilityMatcher")
            .SetParent<Object>()
            .SetGroupName("6g-core")
            .AddConstructor<SixGCapabilityMatcher>();

    return tid;
}

std::vector<SixGCapability>
SixGCapabilityMatcher::Match(
    const SixGCapabilityRequest& request,
    const std::vector<SixGCapability>& candidates) const
{
    std::vector<SixGCapability> matches;

    for (const auto& capability : candidates)
    {
        if (capability.type != request.type)
        {
            continue;
        }

        if (capability.availableResource < request.minimumResource)
        {
            continue;
        }

        if (request.requireAvailable && !capability.available)
        {
            continue;
        }

        matches.push_back(capability);
    }

    std::sort(matches.begin(),
              matches.end(),
              [](const SixGCapability& a, const SixGCapability& b)
              {
                  return a.availableResource > b.availableResource;
              });

    NS_LOG_INFO("Capability matching completed: candidates="
                << candidates.size()
                << ", matches="
                << matches.size());

    return matches;
}

} // namespace ns3
