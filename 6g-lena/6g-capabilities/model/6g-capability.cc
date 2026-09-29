#include "6g-capability.h"

#include "ns3/boolean.h"
#include "ns3/log.h"
#include "ns3/string.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("SixGCapability");

NS_OBJECT_ENSURE_REGISTERED(SixGCapability);

TypeId
SixGCapability::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::SixGCapability")
            .SetParent<Object>()
            .SetGroupName("6g-capabilities")
            .AddConstructor<SixGCapability>()
            .AddAttribute("CapabilityName",
                          "Name identifying the 6G capability.",
                          StringValue("Unknown"),
                          MakeStringAccessor(&SixGCapability::m_capabilityName),
                          MakeStringChecker())
            .AddAttribute("Available",
                          "Whether the capability is currently available.",
                          BooleanValue(true),
                          MakeBooleanAccessor(&SixGCapability::m_available),
                          MakeBooleanChecker());

    return tid;
}

SixGCapability::SixGCapability()
    : m_capabilityName("Unknown"),
      m_available(true),
      m_capabilityType(COMMUNICATION)
{
}

SixGCapability::~SixGCapability() = default;

std::string
SixGCapability::GetCapabilityName() const
{
    return m_capabilityName;
}

void
SixGCapability::SetCapabilityName(const std::string& name)
{
    m_capabilityName = name;
}

bool
SixGCapability::IsAvailable() const
{
    return m_available;
}

void
SixGCapability::SetAvailable(bool available)
{
    m_available = available;
}

void
SixGCapability::SetCapabilityType(CapabilityType type)
{
    m_capabilityType = type;
}

SixGCapability::CapabilityType
SixGCapability::GetCapabilityType() const
{
    return m_capabilityType;
}

} // namespace ns3
