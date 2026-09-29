#ifndef NS3_6G_NET_DEVICE_H
#define NS3_6G_NET_DEVICE_H

#include "ns3/net-device.h"
#include "ns3/6g-capability.h"

#include <vector>

namespace ns3
{

/**
 * \ingroup 6g-devices
 *
 * Base NetDevice abstraction for 6G-LENA.
 */
class SixGNetDevice : public NetDevice
{
  public:
    static TypeId GetTypeId();

    SixGNetDevice();
    ~SixGNetDevice() override;

    void SetIfIndex(const uint32_t index) override;
    uint32_t GetIfIndex() const override;

    Ptr<Channel> GetChannel() const override;

    void SetAddress(Address address) override;
    Address GetAddress() const override;

    bool SetMtu(const uint16_t mtu) override;
    uint16_t GetMtu() const override;

    bool IsLinkUp() const override;
    void AddLinkChangeCallback(Callback<void> callback) override;

    bool IsBroadcast() const override;
    Address GetBroadcast() const override;

    bool IsMulticast() const override;
    Address GetMulticast(Ipv4Address multicastGroup) const override;
    Address GetMulticast(Ipv6Address addr) const override;

    bool IsBridge() const override;
    bool IsPointToPoint() const override;

    bool Send(Ptr<Packet> packet,
              const Address& dest,
              uint16_t protocolNumber) override;

    bool SendFrom(Ptr<Packet> packet,
                  const Address& source,
                  const Address& dest,
                  uint16_t protocolNumber) override;

    Ptr<Node> GetNode() const override;
    void SetNode(Ptr<Node> node) override;

    bool NeedsArp() const override;

    void SetReceiveCallback(NetDevice::ReceiveCallback cb) override;

    void SetPromiscReceiveCallback(PromiscReceiveCallback cb) override;

    bool SupportsSendFrom() const override;

    /**
     * \brief Add a capability to this 6G device.
     */
    void AddCapability(Ptr<SixGCapability> capability);

    /**
     * \brief Get all capabilities provided by this device.
     */
    const std::vector<Ptr<SixGCapability>>& GetCapabilities() const;

    /**
     * \brief Check whether this device provides a capability type.
     */
    bool HasCapability(SixGCapability::CapabilityType type) const;

  private:
    uint32_t m_ifIndex;
    Ptr<Node> m_node;
    Address m_address;
    uint16_t m_mtu;
    NetDevice::ReceiveCallback m_rxCallback;

    std::vector<Ptr<SixGCapability>> m_capabilities;
};

} // namespace ns3

#endif // NS3_6G_NET_DEVICE_H
