#include "6g-service-session.h"
#include "ns3/6g-capability-provider.h"

namespace ns3
{

NS_OBJECT_ENSURE_REGISTERED(SixGServiceSession);

TypeId
SixGServiceSession::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::SixGServiceSession")
            .SetParent<Object>()
            .SetGroupName("6g-lena")
            .AddConstructor<SixGServiceSession>();

    return tid;
}

SixGServiceSession::SixGServiceSession()
    : m_sessionId(""),
      m_ueId(""),
      m_accessNodeId(""),
      m_edgeNodeId(""),
      m_serviceId(""),
      m_active(false),
      m_ue(nullptr),
      m_accessNode(nullptr),
      m_edgeNode(nullptr),
      m_service(nullptr),
      m_capabilityProvider(nullptr),
      m_reservedCommunication(0.0),
      m_reservedCompute(0.0)
{
}

SixGServiceSession::~SixGServiceSession() = default;

void
SixGServiceSession::SetSessionId(const std::string& sessionId)
{
    m_sessionId = sessionId;
}

std::string
SixGServiceSession::GetSessionId() const
{
    return m_sessionId;
}

void
SixGServiceSession::SetUeId(const std::string& ueId)
{
    m_ueId = ueId;
}

std::string
SixGServiceSession::GetUeId() const
{
    return m_ueId;
}

void
SixGServiceSession::SetAccessNodeId(const std::string& accessNodeId)
{
    m_accessNodeId = accessNodeId;
}

std::string
SixGServiceSession::GetAccessNodeId() const
{
    return m_accessNodeId;
}

void
SixGServiceSession::SetEdgeNodeId(const std::string& edgeNodeId)
{
    m_edgeNodeId = edgeNodeId;
}

std::string
SixGServiceSession::GetEdgeNodeId() const
{
    return m_edgeNodeId;
}

void
SixGServiceSession::SetServiceId(const std::string& serviceId)
{
    m_serviceId = serviceId;
}

std::string
SixGServiceSession::GetServiceId() const
{
    return m_serviceId;
}

void
SixGServiceSession::SetActive(bool active)
{
    if (active)
    {
        Activate();
    }
    else
    {
        Deactivate();
    }
}

bool
SixGServiceSession::IsActive() const
{
    return m_active;
}

void
SixGServiceSession::SetUe(Ptr<SixGUe> ue)
{
    m_ue = ue;

    if (m_ue)
    {
        m_ueId = m_ue->GetNodeId();
    }
}

void
SixGServiceSession::SetAccessNode(Ptr<SixGAccessNode> accessNode)
{
    m_accessNode = accessNode;

    if (m_accessNode)
    {
        m_accessNodeId = m_accessNode->GetNodeId();
    }
}

void
SixGServiceSession::SetEdgeNode(Ptr<SixGEdgeNode> edgeNode)
{
    m_edgeNode = edgeNode;

    if (m_edgeNode)
    {
        m_edgeNodeId = m_edgeNode->GetNodeId();
    }
}

void
SixGServiceSession::SetService(Ptr<SixGService> service)
{
    m_service = service;

    if (m_service)
    {
        m_serviceId = m_service->GetServiceId();
    }
}

void
SixGServiceSession::SetCapabilityProvider(
    Ptr<SixGCapabilityProvider> provider)
{
    m_capabilityProvider = provider;
}

bool
SixGServiceSession::Activate()
{
    if (m_active)
    {
        return true;
    }

    if (!m_ue || !m_accessNode || !m_edgeNode || !m_service)
    {
        return false;
    }

    if (!m_ue->IsActive() || !m_accessNode->IsActive() ||
        !m_edgeNode->IsActive() || !m_service->IsActive())
    {
        return false;
    }

    Ptr<SixGAccessNode> servingAccess =
        m_ue->GetConnectedAccessNodeObject();

    if (servingAccess && servingAccess != m_accessNode)
    {
        return false;
    }

    if (!m_accessNode->HasReachableEdgeNode(m_edgeNode))
    {
        return false;
    }

    if (m_accessNode->GetAvailableCommunication() <
        m_service->GetRequiredCommunication())
    {
        return false;
    }

    if (m_edgeNode->GetAvailableCompute() <
        m_service->GetRequiredCompute())
    {
        return false;
    }

    m_reservedCommunication = m_service->GetRequiredCommunication();
    m_reservedCompute = m_service->GetRequiredCompute();

    m_accessNode->SetAvailableCommunication(
        m_accessNode->GetAvailableCommunication() -
        m_reservedCommunication);

    m_edgeNode->SetAvailableCompute(
        m_edgeNode->GetAvailableCompute() -
        m_reservedCompute);

    if (m_capabilityProvider && !m_capabilityProvider->UpdateFromState())
    {
        m_accessNode->SetAvailableCommunication(
            m_accessNode->GetAvailableCommunication() +
            m_reservedCommunication);

        m_edgeNode->SetAvailableCompute(
            m_edgeNode->GetAvailableCompute() +
            m_reservedCompute);

        m_reservedCommunication = 0.0;
        m_reservedCompute = 0.0;

        return false;
    }

    m_ue->SetConnectedAccessNode(m_accessNode);
    m_ue->SetConnected(true);
    m_ue->SetCurrentService(m_service->GetServiceId());

    m_active = true;

    return true;
}

bool
SixGServiceSession::Deactivate()
{
    if (!m_active)
    {
        return true;
    }

    if (!m_accessNode || !m_edgeNode || !m_ue)
    {
        return false;
    }

    /*
     * Release the resources reserved by this session.
     */
    m_accessNode->SetAvailableCommunication(
        m_accessNode->GetAvailableCommunication() +
        m_reservedCommunication);

    m_edgeNode->SetAvailableCompute(
        m_edgeNode->GetAvailableCompute() +
        m_reservedCompute);

    /*
     * Synchronize the capability registry with the
     * restored runtime resource state.
     */
    if (m_capabilityProvider && !m_capabilityProvider->UpdateFromState())
    {
        /*
         * Roll back the resource release if synchronization fails.
         */
        m_accessNode->SetAvailableCommunication(
            m_accessNode->GetAvailableCommunication() -
            m_reservedCommunication);

        m_edgeNode->SetAvailableCompute(
            m_edgeNode->GetAvailableCompute() -
            m_reservedCompute);

        return false;
    }

    m_ue->SetConnectedAccessNode(Ptr<SixGAccessNode>(nullptr));
    m_ue->SetConnected(false);
    m_ue->SetCurrentService("");

    m_reservedCommunication = 0.0;
    m_reservedCompute = 0.0;
    m_active = false;

    return true;
}

} // namespace ns3
