#ifndef NS3_6G_CAPABILITY_H
#define NS3_6G_CAPABILITY_H

#include "ns3/boolean.h"
#include "ns3/object.h"
#include "ns3/string.h"
#include "ns3/uinteger.h"

namespace ns3
{

/**
 * \ingroup 6g-capabilities
 *
 * \brief Base abstraction for a capability provided by a 6G network node.
 */
class SixGCapability : public Object
{
  public:
    /**
     * \brief Type of capability provided by a 6G node.
     */
    enum CapabilityType
    {
        COMMUNICATION,
        COMPUTING,
        SENSING,
        AI
    };

    static TypeId GetTypeId();

    SixGCapability();
    ~SixGCapability() override;

    /**
     * \brief Get the capability name.
     */
    std::string GetCapabilityName() const;

    /**
     * \brief Set the capability name.
     */
    void SetCapabilityName(const std::string& name);

    /**
     * \brief Check whether the capability is available.
     */
    bool IsAvailable() const;

    /**
     * \brief Set capability availability.
     */
    void SetAvailable(bool available);

    /**
     * \brief Set the capability type.
     */
    void SetCapabilityType(CapabilityType type);

    /**
     * \brief Get the capability type.
     */
    CapabilityType GetCapabilityType() const;

  private:
    std::string m_capabilityName;
    bool m_available;
    CapabilityType m_capabilityType;
};

} // namespace ns3

#endif // NS3_6G_CAPABILITY_H
