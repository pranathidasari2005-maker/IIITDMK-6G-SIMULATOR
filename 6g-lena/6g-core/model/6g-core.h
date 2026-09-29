#ifndef NS3_6G_CORE_H
#define NS3_6G_CORE_H

#include "ns3/object.h"
#include "ns3/nstime.h"

#include <map>
#include <string>
#include <vector>

namespace ns3
{

/**
 * @brief Lifecycle state of a 6G capability.
 */
enum class SixGCapabilityState
{
    ACTIVE,
    DEGRADED,
    UNAVAILABLE
};

/**
 * @ingroup 6g-core
 *
 * @brief Describes a capability offered by a 6G node.
 */
struct SixGCapability
{
    std::string nodeId;
    std::string type;

    double capacity{0.0};
    double availableResource{0.0};
    bool available{true};

    /**
     * @brief Current lifecycle state of the capability.
     *
     * The legacy 'available' field is retained temporarily for
     * backward compatibility. The lifecycle state will become
     * the authoritative availability representation in a later
     * migration step.
     */
    SixGCapabilityState state{SixGCapabilityState::ACTIVE};

    /**
     * @brief Simulation time when this capability information was last updated.
     */
    Time lastUpdated{Seconds(0)};

    /**
     * @brief Duration for which this capability information is considered valid.
     */
    Time validityDuration{Seconds(0)};

    /**
     * @brief Optional capability-specific metadata.
     *
     * The common capability model remains independent of the
     * semantics of individual capability types.
     */
    std::map<std::string, std::string> attributes;
};

/**
 * @ingroup 6g-core
 *
 * @brief Describes a capability discovery request.
 */
enum class SixGConstraintOperator
{
    EQUAL,
    NOT_EQUAL,
    GREATER_THAN,
    GREATER_EQUAL,
    LESS_THAN,
    LESS_EQUAL
};

struct SixGCapabilityConstraint
{
    std::string attribute;
    SixGConstraintOperator op{SixGConstraintOperator::EQUAL};
    std::string value;
};

struct SixGCapabilityRequest
{
    std::string type;
    double minimumResource{0.0};
    bool requireAvailable{true};
    bool requireFresh{false};

    /**
     * @brief Capability-specific attributes required by the request.
     *
     * Kept for backward compatibility with the original exact-match
     * discovery mechanism.
     */
    std::map<std::string, std::string> requiredAttributes;

    /**
     * @brief General capability constraints.
     *
     * Constraints support both exact string matching and ordered
     * comparisons over numeric attribute values.
     */
    std::vector<SixGCapabilityConstraint> constraints;
};

/**
 * @ingroup 6g-core
 *
 * @brief Central core object of the 6G-LENA framework.
 *
 * SixGCore provides framework-level configuration, initialization,
 * capability registration, capability updates, and capability
 * discovery for 6G simulations.
 */
class SixGCore : public Object
{
  public:
    static TypeId GetTypeId();

    void SetScenario(const std::string& scenario);
    std::string GetScenario() const;

    void Initialize();
    bool IsInitialized() const;

    bool RegisterCapability(const SixGCapability& capability);

    bool UpdateCapability(const SixGCapability& capability);

    bool HasCapability(const std::string& nodeId,
                      const std::string& type) const;

    std::vector<SixGCapability> GetCapabilities() const;

    bool RemoveCapability(const std::string& nodeId,
                          const std::string& type);

    std::vector<SixGCapability>
    DiscoverCapabilities(const SixGCapabilityRequest& request) const;

    /**
     * @brief Discover capabilities among a topology-defined set of nodes.
     *
     * The caller supplies the node IDs that are reachable within the
     * current network context. SixGCore remains topology-agnostic and
     * performs capability matching only within this candidate set.
     */
    std::vector<SixGCapability>
    DiscoverCapabilities(const SixGCapabilityRequest& request,
                         const std::vector<std::string>& candidateNodeIds) const;

    /**
     * @brief Check whether capability information is still fresh.
     *
     * A validity duration of zero disables expiration checking.
     */
    bool IsCapabilityFresh(const SixGCapability& capability) const;

  private:
    std::string m_scenario;
    bool m_initialized{false};

    std::vector<SixGCapability> m_capabilities;
};

} // namespace ns3

#endif // NS3_6G_CORE_H
