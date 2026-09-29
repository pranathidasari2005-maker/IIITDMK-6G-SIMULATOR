#include "6g-net-device.h"

#include "ns3/address.h"
#include "ns3/boolean.h"
#include "ns3/channel.h"
#include "ns3/log.h"
#include "ns3/node.h"
#include "ns3/packet.h"
#include "ns3/ptr.h"
#include "ns3/type-id.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("SixGNetDevice");

NS_OBJECT_ENSURE_REGISTERED(SixGNetDevice);

TypeId
SixGNetDevice::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::SixGNetDevice")
            .SetParent<NetDevice>()
            .SetGroupName("6g-devices")
            .AddConstructor<SixGNetDevice>();

    return tid;
}

SixGNetDevice::SixGNetDevice()
    : m_ifIndex(0),
      m_node(nullptr),
      m_mtu(1500)
{
}

SixGNetDevice::~SixGNetDevice()
{
}

void
SixGNetDevice::SetIfIndex(const uint32_t index)
{
    m_ifIndex = index;
}

uint32_t
SixGNetDevice::GetIfIndex() const
{
    return m_ifIndex;
}

Ptr<Channel>
SixGNetDevice::GetChannel() const
{
    return nullptr;
}

void
SixGNetDevice::SetAddress(Address address)
{
    m_address = address;
}

Address
SixGNetDevice::GetAddress() const
{
    return m_address;
}

bool
SixGNetDevice::SetMtu(const uint16_t mtu)
{
    m_mtu = mtu;
    return true;
}

uint16_t
SixGNetDevice::GetMtu() const
{
    return m_mtu;
}

bool
SixGNetDevice::IsLinkUp() const
{
    return true;
}

void
SixGNetDevice::AddLinkChangeCallback(Callback<void> callback)
{
}

bool
SixGNetDevice::IsBroadcast() const
{
    return true;
}

Address
SixGNetDevice::GetBroadcast() const
{
    return Mac48Address::GetBroadcast();
}

bool
SixGNetDevice::IsMulticast() const
{
    return true;
}

Address
SixGNetDevice::GetMulticast(Ipv4Address multicastGroup) const
{
    return Mac48Address::GetMulticast(multicastGroup);
}

Address
SixGNetDevice::GetMulticast(Ipv6Address addr) const
{
    return Mac48Address::GetMulticast(addr);
}

bool
SixGNetDevice::IsBridge() const
{
    return false;
}

bool
SixGNetDevice::IsPointToPoint() const
{
    return false;
}

bool
SixGNetDevice::Send(Ptr<Packet> packet,
                    const Address& dest,
                    uint16_t protocolNumber)
{
    return false;
}

bool
SixGNetDevice::SendFrom(Ptr<Packet> packet,
                        const Address& source,
                        const Address& dest,
                        uint16_t protocolNumber)
{
    return false;
}

Ptr<Node>
SixGNetDevice::GetNode() const
{
    return m_node;
}

void
SixGNetDevice::SetNode(Ptr<Node> node)
{
    m_node = node;
}

bool
SixGNetDevice::NeedsArp() const
{
    return false;
}

void
SixGNetDevice::SetReceiveCallback(NetDevice::ReceiveCallback cb)
{
    m_rxCallback = cb;
}

void
SixGNetDevice::SetPromiscReceiveCallback(PromiscReceiveCallback cb)
{
}

bool
SixGNetDevice::SupportsSendFrom() const
{
    return false;
}

void
SixGNetDevice::AddCapability(Ptr<SixGCapability> capability)
{
    if (capability)
    {
        m_capabilities.push_back(capability);
    }
}

const std::vector<Ptr<SixGCapability>>&
SixGNetDevice::GetCapabilities() const
{
    return m_capabilities;
}

bool
SixGNetDevice::HasCapability(SixGCapability::CapabilityType type) const
{
    for (const auto& capability : m_capabilities)
    {
        if (capability &&
            capability->GetCapabilityType() == type &&
            capability->IsAvailable())
        {
            return true;
        }
    }

    return false;
}

} // namespace ns3
