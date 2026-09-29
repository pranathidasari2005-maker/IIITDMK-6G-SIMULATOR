#ifndef NS3_6G_RESOURCE_H
#define NS3_6G_RESOURCE_H

#include "ns3/object.h"

#include <string>

namespace ns3
{

class SixGResource : public Object
{
  public:
    static TypeId GetTypeId();

    SixGResource();
    ~SixGResource() override;

    void SetType(const std::string& type);
    std::string GetType() const;

    bool SetCapacity(double capacity);
    double GetCapacity() const;

    bool SetAvailable(double available);
    double GetAvailable() const;

    double GetUtilization() const;

    bool Consume(double amount);
    bool Release(double amount);

  private:
    std::string m_type;
    double m_capacity{0.0};
    double m_available{0.0};
};

} // namespace ns3

#endif // NS3_6G_RESOURCE_H
