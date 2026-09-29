#include "6g-state-monitor.h"

#include "ns3/log.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("SixGStateMonitor");

TypeId
SixGStateMonitor::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::SixGStateMonitor")
            .SetParent<Object>()
            .SetGroupName("6g-core");

    return tid;
}

void
SixGStateMonitor::SetNodeId(const std::string& nodeId)
{
    m_nodeId = nodeId;
}

std::string
SixGStateMonitor::GetNodeId() const
{
    return m_nodeId;
}

void
SixGStateMonitor::SetComputeCapacity(double capacity)
{
    if (capacity < 0.0)
    {
        return;
    }

    m_computeCapacity = capacity;

    if (m_availableCompute > m_computeCapacity)
    {
        m_availableCompute = m_computeCapacity;
    }
}

double
SixGStateMonitor::GetComputeCapacity() const
{
    return m_computeCapacity;
}

void
SixGStateMonitor::SetAvailableCompute(double available)
{
    if (available < 0.0 || available > m_computeCapacity)
    {
        return;
    }

    m_availableCompute = available;
}

double
SixGStateMonitor::GetAvailableCompute() const
{
    return m_availableCompute;
}

void
SixGStateMonitor::SetCommunicationCapacity(double capacity)
{
    if (capacity < 0.0)
    {
        return;
    }

    m_communicationCapacity = capacity;

    if (m_availableCommunication > m_communicationCapacity)
    {
        m_availableCommunication = m_communicationCapacity;
    }
}

double
SixGStateMonitor::GetCommunicationCapacity() const
{
    return m_communicationCapacity;
}

void
SixGStateMonitor::SetAvailableCommunication(double available)
{
    if (available < 0.0 || available > m_communicationCapacity)
    {
        return;
    }

    m_availableCommunication = available;
}

double
SixGStateMonitor::GetAvailableCommunication() const
{
    return m_availableCommunication;
}

void
SixGStateMonitor::SetConnected(bool connected)
{
    m_connected = connected;
}

bool
SixGStateMonitor::IsConnected() const
{
    return m_connected;
}

void
SixGStateMonitor::SetApplicationLoad(double load)
{
    if (load < 0.0)
    {
        return;
    }

    m_applicationLoad = load;
}

double
SixGStateMonitor::GetApplicationLoad() const
{
    return m_applicationLoad;
}

void
SixGStateMonitor::SetPosition(const Vector& position)
{
    m_position = position;
}

Vector
SixGStateMonitor::GetPosition() const
{
    return m_position;
}

void
SixGStateMonitor::SetMobilityModel(Ptr<MobilityModel> mobility)
{
    m_mobility = mobility;
}

bool
SixGStateMonitor::UpdateFromMobility()
{
    if (!m_mobility)
    {
        return false;
    }

    m_position = m_mobility->GetPosition();
    return true;
}

void
SixGStateMonitor::SetCommunicationQuality(double quality)
{
    if (quality < 0.0 || quality > 1.0)
    {
        return;
    }

    m_communicationQuality = quality;
}

double
SixGStateMonitor::GetCommunicationQuality() const
{
    return m_communicationQuality;
}

bool
SixGStateMonitor::UpdateCommunicationFromQuality()
{
    if (m_communicationCapacity < 0.0)
    {
        return false;
    }

    if (m_communicationQuality < 0.0 ||
        m_communicationQuality > 1.0)
    {
        return false;
    }

    m_availableCommunication =
        m_communicationCapacity * m_communicationQuality;

    return true;
}

} // namespace ns3
