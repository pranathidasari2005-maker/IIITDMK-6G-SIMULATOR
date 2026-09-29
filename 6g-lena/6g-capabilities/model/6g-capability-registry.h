#ifndef NS3_6G_CAPABILITY_REGISTRY_H
#define NS3_6G_CAPABILITY_REGISTRY_H

#include "ns3/6g-capability.h"
#include "ns3/node.h"
#include "ns3/object.h"
#include "ns3/ptr.h"

#include <vector>

namespace ns3
{

/**
 * \ingroup 6g-capabilities
 *
 * \brief Registry for capabilities provided by 6G network nodes.
 */
class SixGCapabilityRegistry : public Object
{
  public:
    /**
     * \brief A capability together with its provider node.
     */
    struct CapabilityRegistration
    {
        Ptr<Node> node;
        Ptr<SixGCapability> capability;
    };

    static TypeId GetTypeId();

    SixGCapabilityRegistry();
    ~SixGCapabilityRegistry() override;

    /**
     * \brief Register a capability provided by a node.
     */
    void RegisterCapability(Ptr<Node> node,
                            Ptr<SixGCapability> capability);

    /**
     * \brief Remove a capability from the registry.
     *
     * \return true if the capability was removed.
     */
    bool RemoveCapability(Ptr<Node> node,
                          Ptr<SixGCapability> capability);

    /**
     * \brief Find capabilities of a given type.
     *
     * \return Matching capability registrations.
     */
    std::vector<CapabilityRegistration>
    FindCapabilitiesByType(SixGCapability::CapabilityType type) const;

    /**
     * \brief Get all registered capabilities.
     *
     * \return All capability registrations.
     */
    std::vector<CapabilityRegistration>
    GetRegistrations() const;

    /**
     * \brief Get the number of registered capabilities.
     */
    uint32_t GetCapabilityCount() const;

  private:
    std::vector<CapabilityRegistration> m_registrations;
};

} // namespace ns3

#endif // NS3_6G_CAPABILITY_REGISTRY_H
