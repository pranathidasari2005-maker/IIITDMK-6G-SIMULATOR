#include "6g-resource.h"

#include "ns3/log.h"

#include <cmath>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("SixGResource");

NS_OBJECT_ENSURE_REGISTERED(SixGResource);

TypeId
SixGResource::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::SixGResource")
            .SetParent<Object>()
            .SetGroupName("6g-network-nodes")
            .AddConstructor<SixGResource>();
    return tid;
}

SixGResource::SixGResource() = default;

SixGResource::~SixGResource() = default;

void
SixGResource::SetType(const std::string& type)
{
    m_type = type;
}

std::string
SixGResource::GetType() const
{
    return m_type;
}

bool
SixGResource::SetCapacity(double capacity)
{
    if (!std::isfinite(capacity) || capacity < 0.0)
    {
        return false;
    }

    if (capacity < m_capacity - m_available)
    {
        return false;
    }

    m_capacity = capacity;
    return true;
}

double
SixGResource::GetCapacity() const
{
    return m_capacity;
}

bool
SixGResource::SetAvailable(double available)
{
    if (!std::isfinite(available) || available < 0.0 || available > m_capacity)
    {
        return false;
    }

    m_available = available;
    return true;
}

double
SixGResource::GetAvailable() const
{
    return m_available;
}

double
SixGResource::GetUtilization() const
{
    if (m_capacity <= 0.0)
    {
        return 0.0;
    }

    return (m_capacity - m_available) / m_capacity;
}

bool
SixGResource::Consume(double amount)
{
    if (!std::isfinite(amount) || amount < 0.0)
    {
        return false;
    }

    if (amount > m_available)
    {
        return false;
    }

    m_available -= amount;
    return true;
}

bool
SixGResource::Release(double amount)
{
    if (!std::isfinite(amount) || amount < 0.0)
    {
        return false;
    }

    if (m_available + amount > m_capacity)
    {
        return false;
    }

    m_available += amount;
    return true;
}

} // namespace ns3
