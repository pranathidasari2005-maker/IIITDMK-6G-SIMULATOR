#ifndef NS3_SIXG_SERVICE_H
#define NS3_SIXG_SERVICE_H

#include "ns3/object.h"

#include <string>

namespace ns3
{

/**
 * \ingroup network-nodes
 *
 * 6G service abstraction.
 */
class SixGService : public Object
{
  public:
    static TypeId GetTypeId();

    SixGService();
    ~SixGService() override;

    void SetServiceId(const std::string& serviceId);
    std::string GetServiceId() const;

    void SetRequiredCompute(double compute);
    double GetRequiredCompute() const;

    void SetRequiredCommunication(double communication);
    double GetRequiredCommunication() const;

    void SetMaximumLatency(double latency);
    double GetMaximumLatency() const;

    void SetMinimumReliability(double reliability);
    double GetMinimumReliability() const;

    void SetActive(bool active);
    bool IsActive() const;

  private:
    std::string m_serviceId;
    double m_requiredCompute;
    double m_requiredCommunication;
    double m_maximumLatency;
    double m_minimumReliability;
    bool m_active;
};

} // namespace ns3

#endif // NS3_SIXG_SERVICE_H
