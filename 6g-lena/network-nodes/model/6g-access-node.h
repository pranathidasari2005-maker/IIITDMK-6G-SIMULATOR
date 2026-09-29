#ifndef NS3_SIXG_ACCESS_NODE_H
#define NS3_SIXG_ACCESS_NODE_H

#include "6g-network-node.h"

#include <string>
#include <vector>

namespace ns3
{

class SixGEdgeNode;

/**
 * \ingroup network-nodes
 *
 * 6G Access Node abstraction.
 */
class SixGAccessNode : public SixGNode
{
  public:
    static TypeId GetTypeId();

    SixGAccessNode();
    ~SixGAccessNode() override;

    void AddConnectedUe(const std::string& ueId);
    void RemoveConnectedUe(const std::string& ueId);
    uint32_t GetConnectedUeCount() const;
    bool HasConnectedUe(const std::string& ueId) const;

    void AddReachableEdgeNode(Ptr<SixGEdgeNode> edgeNode);
    void RemoveReachableEdgeNode(Ptr<SixGEdgeNode> edgeNode);
    bool HasReachableEdgeNode(Ptr<SixGEdgeNode> edgeNode) const;
    uint32_t GetReachableEdgeNodeCount() const;
    Ptr<SixGEdgeNode> GetReachableEdgeNode(uint32_t index) const;

    void SetCommunicationCapacity(double capacity);
    double GetCommunicationCapacity() const;

    void SetAvailableCommunication(double available);
    double GetAvailableCommunication() const;

    void SetCoverageRadius(double radius);
    double GetCoverageRadius() const;

    void SetLoad(double load);
    double GetLoad() const;

  private:
    std::vector<std::string> m_connectedUes;
    std::vector<Ptr<SixGEdgeNode>> m_reachableEdgeNodes;
    double m_communicationCapacity;
    double m_availableCommunication;
    double m_coverageRadius;
    double m_load;
};

} // namespace ns3

#endif // NS3_SIXG_ACCESS_NODE_H
