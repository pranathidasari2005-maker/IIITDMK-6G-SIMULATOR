#ifndef NS3_SIXG_UE_H
#define NS3_SIXG_UE_H

#include "6g-network-node.h"

#include <string>

namespace ns3
{

class SixGAccessNode;

/**
 * \ingroup network-nodes
 *
 * 6G User Equipment (UE) abstraction.
 */
class SixGUe : public SixGNode
{
  public:
    static TypeId GetTypeId();

    SixGUe();
    ~SixGUe() override;

    void SetConnectedAccessNode(const std::string& accessNodeId);
    std::string GetConnectedAccessNode() const;
    void SetConnectedAccessNode(Ptr<SixGAccessNode> accessNode);
    Ptr<SixGAccessNode> GetConnectedAccessNodeObject() const;

    void SetConnected(bool connected);
    bool IsConnected() const;

    void SetCurrentService(const std::string& serviceId);
    std::string GetCurrentService() const;

  private:
    std::string m_connectedAccessNode;
    Ptr<SixGAccessNode> m_connectedAccessNodeObject;
    bool m_connected;
    std::string m_currentService;
};

} // namespace ns3

#endif // NS3_SIXG_UE_H
