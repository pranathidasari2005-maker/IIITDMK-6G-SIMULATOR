#include "6g-service.h"

namespace ns3
{

NS_OBJECT_ENSURE_REGISTERED(SixGService);

TypeId
SixGService::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::SixGService")
            .SetParent<Object>()
            .SetGroupName("6g-lena")
            .AddConstructor<SixGService>();

    return tid;
}

SixGService::SixGService()
    : m_serviceId(""),
      m_requiredCompute(0.0),
      m_requiredCommunication(0.0),
      m_maximumLatency(0.0),
      m_minimumReliability(0.0),
      m_active(false)
{
}

SixGService::~SixGService() = default;

void
SixGService::SetServiceId(const std::string& serviceId)
{
    m_serviceId = serviceId;
}

std::string
SixGService::GetServiceId() const
{
    return m_serviceId;
}

void
SixGService::SetRequiredCompute(double compute)
{
    m_requiredCompute = compute;
}

double
SixGService::GetRequiredCompute() const
{
    return m_requiredCompute;
}

void
SixGService::SetRequiredCommunication(double communication)
{
    m_requiredCommunication = communication;
}

double
SixGService::GetRequiredCommunication() const
{
    return m_requiredCommunication;
}

void
SixGService::SetMaximumLatency(double latency)
{
    m_maximumLatency = latency;
}

double
SixGService::GetMaximumLatency() const
{
    return m_maximumLatency;
}

void
SixGService::SetMinimumReliability(double reliability)
{
    m_minimumReliability = reliability;
}

double
SixGService::GetMinimumReliability() const
{
    return m_minimumReliability;
}

void
SixGService::SetActive(bool active)
{
    m_active = active;
}

bool
SixGService::IsActive() const
{
    return m_active;
}

} // namespace ns3
