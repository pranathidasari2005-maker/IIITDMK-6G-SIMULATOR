#ifndef NS3_6G_COMPUTE_TASK_H
#define NS3_6G_COMPUTE_TASK_H

#include "6g-capability-provider.h"
#include "ns3/6g-resource.h"

#include "ns3/object.h"
#include "ns3/nstime.h"

namespace ns3
{

/**
 * @ingroup 6g-core
 *
 * @brief Represents a simulated computing task.
 *
 * A task consumes a generic COMPUTING resource from a capability provider
 * and optionally from a node resource registry when it starts.
 */
class SixGComputeTask : public Object
{
  public:
    static TypeId GetTypeId();

    /**
     * @brief Set the capability provider that executes this task.
     */
    void SetProvider(Ptr<SixGCapabilityProvider> provider);

    /**
     * @brief Set the computing resource used by this task.
     */
    void SetComputeResource(Ptr<SixGResource> resource);

    /**
     * @brief Set the amount of computing resource required.
     */
    void SetRequiredResource(double resource);

    /**
     * @brief Start the task.
     *
     * Required resource is consumed from the configured resource
     * and capability provider.
     */
    bool Start();

    /**
     * @brief Finish the task.
     *
     * Previously consumed resource is released.
     */
    bool Finish();

    /**
     * @brief Schedule the task to start after a delay.
     */
    void ScheduleStart(Time delay);

    /**
     * @brief Schedule the task to finish after a delay.
     */
    void ScheduleFinish(Time delay);

    /**
     * @brief Check whether the task is currently running.
     */
    bool IsRunning() const;

  private:
    Ptr<SixGCapabilityProvider> m_provider;
    Ptr<SixGResource> m_computeResource;

    double m_requiredResource{0.0};

    bool m_running{false};
};

} // namespace ns3

#endif // NS3_6G_COMPUTE_TASK_H
