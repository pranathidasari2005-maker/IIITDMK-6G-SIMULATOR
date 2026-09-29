#include "6g-ue.h"
#include "6g-access-node.h"

#include "ns3/log.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("SixGUe");

NS_OBJECT_ENSURE_REGISTERED(SixGUe);

TypeId
SixGUe::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::SixGUe")
            .SetParent<SixGNode>()
            .SetGroupName("6g-lena")
            .AddConstructor<SixGUe>();

    return tid;
}

SixGUe::SixGUe()
    : m_connectedAccessNode(""),
      m_connectedAccessNodeObject(nullptr),
      m_connected(false),
      m_currentService("")
{
}

SixGUe::~SixGUe() = default;

void
SixGUe::SetConnectedAccessNode(const std::string& accessNodeId)
{
    m_connectedAccessNode = accessNodeId;
}

std::string
SixGUe::GetConnectedAccessNode() const
{
    return m_connectedAccessNode;
}

void
SixGUe::SetConnectedAccessNode(Ptr<SixGAccessNode> accessNode)
{
    /*
     * A null Access Node means detach.
     */
    if (!accessNode)
    {
        if (m_connectedAccessNodeObject)
        {
            m_connectedAccessNodeObject->RemoveConnectedUe(GetNodeId());
        }

        m_connectedAccessNodeObject = nullptr;
        m_connectedAccessNode.clear();
        m_connected = false;
        return;
    }

    /*
     * If already attached to this Access Node, do nothing.
     */
    if (m_connectedAccessNodeObject == accessNode)
    {
        return;
    }

    /*
     * Remove the UE from its previous serving Access Node.
     */
    if (m_connectedAccessNodeObject)
    {
        m_connectedAccessNodeObject->RemoveConnectedUe(GetNodeId());
    }

    m_connectedAccessNodeObject = accessNode;
    m_connectedAccessNode = accessNode->GetNodeId();
    m_connected = true;

    /*
     * Keep the Access Node's UE list synchronized with the UE state.
     */
    accessNode->AddConnectedUe(GetNodeId());
}

Ptr<SixGAccessNode>
SixGUe::GetConnectedAccessNodeObject() const
{
    return m_connectedAccessNodeObject;
}

void
SixGUe::SetConnected(bool connected)
{
    m_connected = connected;
}

bool
SixGUe::IsConnected() const
{
    return m_connected;
}

void
SixGUe::SetCurrentService(const std::string& serviceId)
{
    m_currentService = serviceId;
}

std::string
SixGUe::GetCurrentService() const
{
    return m_currentService;
}

} // namespace ns3
