#ifndef NS3_6G_SERVICE_SESSION_H
#define NS3_6G_SERVICE_SESSION_H

#include "6g-access-node.h"
#include "6g-edge-node.h"
#include "6g-service.h"
#include "6g-ue.h"

#include "ns3/object.h"

#include <string>

namespace ns3
{
class SixGCapabilityProvider;


/**
 * \ingroup network-nodes
 *
 * Represents an active 6G service session connecting a UE,
 * access node, edge node, and service.
 *
 * The session can reserve and release communication and
 * computing resources from the participating simulated nodes.
 */
class SixGServiceSession : public Object
{
  public:
    static TypeId GetTypeId();

    SixGServiceSession();
    ~SixGServiceSession() override;

    void SetSessionId(const std::string& sessionId);
    std::string GetSessionId() const;

    void SetUeId(const std::string& ueId);
    std::string GetUeId() const;

    void SetAccessNodeId(const std::string& accessNodeId);
    std::string GetAccessNodeId() const;

    void SetEdgeNodeId(const std::string& edgeNodeId);
    std::string GetEdgeNodeId() const;

    void SetServiceId(const std::string& serviceId);
    std::string GetServiceId() const;

    void SetActive(bool active);
    bool IsActive() const;

    /**
     * @brief Bind the session to simulated network entities.
     */
    void SetUe(Ptr<SixGUe> ue);
    void SetAccessNode(Ptr<SixGAccessNode> accessNode);
    void SetEdgeNode(Ptr<SixGEdgeNode> edgeNode);
    void SetService(Ptr<SixGService> service);
    void SetCapabilityProvider(Ptr<SixGCapabilityProvider> provider);

    /**
     * @brief Activate the service session and reserve resources.
     *
     * Returns false if the required resources are unavailable.
     */
    bool Activate();

    /**
     * @brief Deactivate the session and release its reserved resources.
     */
    bool Deactivate();

  private:
    std::string m_sessionId;
    std::string m_ueId;
    std::string m_accessNodeId;
    std::string m_edgeNodeId;
    std::string m_serviceId;

    bool m_active;

    Ptr<SixGUe> m_ue;
    Ptr<SixGAccessNode> m_accessNode;
    Ptr<SixGEdgeNode> m_edgeNode;
    Ptr<SixGService> m_service;
    Ptr<SixGCapabilityProvider> m_capabilityProvider;

    double m_reservedCommunication;
    double m_reservedCompute;
};

} // namespace ns3

#endif // NS3_6G_SERVICE_SESSION_H
