#ifndef NS3_6G_CAPABILITY_PROVIDER_H
#define NS3_6G_CAPABILITY_PROVIDER_H

#include "6g-core.h"

#include "ns3/object.h"
#include "ns3/6g-resource.h"
#include "ns3/6g-state-monitor.h"

namespace ns3
{

/**
 * @ingroup 6g-core
 *
 * @brief Represents a simulated node that provides a capability.
 *
 * The provider connects a generic SixGResource to the capability
 * discovery and registry mechanisms. When a resource is attached,
 * the resource is the authoritative source for capacity and
 * availability.
 */
class SixGCapabilityProvider : public Object
{
  public:
    static TypeId GetTypeId();

    void SetCore(Ptr<SixGCore> core);

    /**
     * @brief Attach a simulation state monitor to this capability provider.
     *
     * The state monitor provides dynamic node state such as mobility
     * and connectivity. Resource quantities are taken from the attached
     * SixGResource when one is available.
     */
    void SetStateMonitor(Ptr<SixGStateMonitor> monitor);

    /**
     * @brief Attach the generic resource represented by this capability.
     */
    void SetResource(Ptr<SixGResource> resource);

    /**
     * @brief Update the capability from the current simulated state.
     */
    bool UpdateFromState();

    void SetNodeId(const std::string& nodeId);
    void SetCapabilityType(const std::string& type);

    /**
     * @brief Set the total capacity of the capability.
     *
     * When a generic resource is attached, the resource capacity is
     * updated and becomes the authoritative capacity.
     */
    void SetCapacity(double capacity);

    /**
     * @brief Set the current available resource.
     *
     * When a generic resource is attached, its available amount is
     * updated directly.
     */
    void SetAvailableResource(double availableResource);

    void SetAvailable(bool available);

    /**
     * @brief Set the lifecycle state of the capability.
     */
    void SetState(SixGCapabilityState state);

    /**
     * @brief Get the current lifecycle state of the capability.
     */
    SixGCapabilityState GetState() const;

    /**
     * @brief Set the duration for which capability information is valid.
     *
     * A zero duration disables freshness expiration.
     */
    void SetValidityDuration(Time duration);

    /**
     * @brief Add or update capability-specific metadata.
     */
    void SetAttribute(const std::string& name,
                      const std::string& value);

    /**
     * @brief Retrieve capability-specific metadata.
     *
     * Returns an empty string when the attribute does not exist.
     */
    std::string GetAttribute(const std::string& name) const;

    bool Register();

    /**
     * @brief Consume available capability resource.
     */
    bool ConsumeResource(double amount);

    /**
     * @brief Release previously consumed capability resource.
     */
    bool ReleaseResource(double amount);

    Ptr<SixGResource> GetResource() const;
    SixGCapability GetCapability() const;

  private:
    Ptr<SixGStateMonitor> m_stateMonitor;
    Ptr<SixGResource> m_resource;
    Ptr<SixGCore> m_core;
    SixGCapability m_capability;
};

} // namespace ns3

#endif // NS3_6G_CAPABILITY_PROVIDER_H
