#ifndef NS3_NETWORK_NODES_H
#define NS3_NETWORK_NODES_H

#include "6g-network-node.h"
#include "ns3/6g-capability.h"

#include <vector>

namespace ns3
{

/**
 * \ingroup network-nodes
 *
 * Compatibility header for the 6G network-node module.
 *
 * SixGNode is defined in 6g-network-node.h.
 */
using SixGCapabilityList = std::vector<Ptr<SixGCapability>>;

} // namespace ns3

#endif // NS3_NETWORK_NODES_H
