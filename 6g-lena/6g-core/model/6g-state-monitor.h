#ifndef NS3_6G_STATE_MONITOR_H
#define NS3_6G_STATE_MONITOR_H

#include "ns3/object.h"
#include "ns3/vector.h"
#include "ns3/mobility-model.h"

#include <string>

namespace ns3
{

/**
 * @brief Represents the simulation state relevant to 6G capabilities.
 *
 * This class provides a simulation-native state abstraction between
 * the 6G network/application simulation and the capability layer.
 *
 * The first implementation is intentionally generic. Concrete
 * ns-3/6G network components will populate this state in later steps.
 */
class SixGStateMonitor : public Object
{
  public:
    static TypeId GetTypeId();

    void SetNodeId(const std::string& nodeId);
    std::string GetNodeId() const;

    void SetComputeCapacity(double capacity);
    double GetComputeCapacity() const;

    void SetAvailableCompute(double available);
    double GetAvailableCompute() const;

    void SetCommunicationCapacity(double capacity);
    double GetCommunicationCapacity() const;

    void SetAvailableCommunication(double available);
    double GetAvailableCommunication() const;

    void SetCommunicationQuality(double quality);
    double GetCommunicationQuality() const;

    bool UpdateCommunicationFromQuality();

    void SetDownlinkSinr(double sinr);
    double GetDownlinkSinr() const;

    void SetConnected(bool connected);
    bool IsConnected() const;

    void SetApplicationLoad(double load);
    double GetApplicationLoad() const;

    /**
     * @brief Record the current simulated mobility position.
     */
    void SetPosition(const Vector& position);

    /**
     * @brief Get the current simulated mobility position.
     */
    Vector GetPosition() const;

    /**
     * @brief Attach the ns-3 mobility model observed by this monitor.
     */
    void SetMobilityModel(Ptr<MobilityModel> mobility);

    /**
     * @brief Update the stored position from the current simulation state.
     */
    bool UpdateFromMobility();

  private:
    std::string m_nodeId;

    double m_computeCapacity{0.0};
    double m_availableCompute{0.0};

    double m_communicationCapacity{0.0};
    double m_availableCommunication{0.0};
    double m_communicationQuality{0.0};
    double m_downlinkSinr{0.0};

    bool m_connected{true};

    double m_applicationLoad{0.0};

    Vector m_position;
    Ptr<MobilityModel> m_mobility;
};

} // namespace ns3

#endif // NS3_6G_STATE_MONITOR_H
