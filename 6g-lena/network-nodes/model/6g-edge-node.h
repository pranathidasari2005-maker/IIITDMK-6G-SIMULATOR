#ifndef NS3_SIXG_EDGE_NODE_H
#define NS3_SIXG_EDGE_NODE_H

#include "6g-network-node.h"

namespace ns3
{


/**
 * \ingroup network-nodes
 *
 * 6G Edge Computing Node abstraction.
 */
class SixGEdgeNode : public SixGNode
{
  public:
    static TypeId GetTypeId();

    SixGEdgeNode();
    ~SixGEdgeNode() override;

    void SetComputeCapacity(double capacity);
    double GetComputeCapacity() const;

    void SetAvailableCompute(double available);
    double GetAvailableCompute() const;

    bool ConsumeCompute(double amount);
    bool ReleaseCompute(double amount);

    void SetConnectedCoreId(const std::string& coreId);
    std::string GetConnectedCoreId() const;

    void SetMemoryCapacity(double capacity);
    double GetMemoryCapacity() const;

    void SetAvailableMemory(double available);
    double GetAvailableMemory() const;

    void SetApplicationLoad(double load);
    double GetApplicationLoad() const;

  private:
    std::string m_connectedCoreId;
    double m_computeCapacity;
    double m_availableCompute;
    double m_memoryCapacity;
    double m_availableMemory;
    double m_applicationLoad;
};

} // namespace ns3

#endif // NS3_SIXG_EDGE_NODE_H
