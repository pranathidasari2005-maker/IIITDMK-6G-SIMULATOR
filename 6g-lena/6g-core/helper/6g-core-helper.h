#ifndef NS3_6G_CORE_HELPER_H
#define NS3_6G_CORE_HELPER_H

#include "ns3/6g-core.h"

#include <string>

namespace ns3
{

/**
 * @ingroup 6g-core
 *
 * @brief Helper for creating and configuring the 6G-LENA core.
 */
class SixGCoreHelper
{
  public:
    /**
     * @brief Create a 6G-LENA core with the selected scenario.
     *
     * @param scenario Name of the simulation scenario.
     * @return Configured SixGCore object.
     */
    Ptr<SixGCore> CreateCore(const std::string& scenario) const;
};

} // namespace ns3

#endif // NS3_6G_CORE_HELPER_H
