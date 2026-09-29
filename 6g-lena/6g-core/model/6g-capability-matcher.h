#ifndef NS3_6G_CAPABILITY_MATCHER_H
#define NS3_6G_CAPABILITY_MATCHER_H

#include "6g-core.h"

#include "ns3/object.h"

#include <vector>

namespace ns3
{

/**
 * @ingroup 6g-core
 *
 * @brief Ranks discovered capabilities according to resource suitability.
 *
 * This class provides a baseline capability matching mechanism.
 * Candidate capabilities are expected to be obtained from the
 * capability discovery stage.
 */
class SixGCapabilityMatcher : public Object
{
  public:
    static TypeId GetTypeId();

    /**
     * @brief Rank capability candidates for a given request.
     *
     * Candidates that satisfy the request are returned in descending
     * order of available resource.
     */
    std::vector<SixGCapability>
    Match(const SixGCapabilityRequest& request,
          const std::vector<SixGCapability>& candidates) const;
};

} // namespace ns3

#endif // NS3_6G_CAPABILITY_MATCHER_H
