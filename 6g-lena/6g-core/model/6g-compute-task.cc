#include "6g-compute-task.h"

#include "ns3/log.h"
#include "ns3/simulator.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("SixGComputeTask");

NS_OBJECT_ENSURE_REGISTERED(SixGComputeTask);

TypeId
SixGComputeTask::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::SixGComputeTask")
            .SetParent<Object>()
            .SetGroupName("6g-core")
            .AddConstructor<SixGComputeTask>();
    return tid;
}

void
SixGComputeTask::SetProvider(Ptr<SixGCapabilityProvider> provider)
{
    m_provider = provider;
    m_computeResource = provider ? provider->GetResource() : nullptr;
}

void
SixGComputeTask::SetComputeResource(Ptr<SixGResource> resource)
{
    m_computeResource = resource;
}

void
SixGComputeTask::SetRequiredResource(double resource)
{
    m_requiredResource = resource;
}

bool
SixGComputeTask::Start()
{
    if (!m_provider || !m_computeResource || m_running || m_requiredResource <= 0.0)
    {
        return false;
    }

    if (m_computeResource->GetType() != "COMPUTING")
    {
        return false;
    }

    if (!m_computeResource->Consume(m_requiredResource))
    {
        return false;
    }

    /*
     * Propagate the resource change to the capability registry.
     * If synchronization fails, restore the consumed resource so
     * the task start remains atomic.
     */
    if (!m_provider->UpdateFromState())
    {
        m_computeResource->Release(m_requiredResource);
        return false;
    }

    m_running = true;

    NS_LOG_INFO("Compute task started. Required resource: "
                << m_requiredResource
                << ", available resource: "
                << m_computeResource->GetAvailable());

    return true;
}

bool
SixGComputeTask::Finish()
{
    if (!m_provider || !m_computeResource || !m_running)
    {
        return false;
    }

    if (!m_computeResource->Release(m_requiredResource))
    {
        return false;
    }

    /*
     * Propagate the released resource back to the capability registry.
     * If synchronization fails, restore the previous resource state.
     */
    if (!m_provider->UpdateFromState())
    {
        m_computeResource->Consume(m_requiredResource);
        return false;
    }

    m_running = false;

    NS_LOG_INFO("Compute task finished. Released resource: "
                << m_requiredResource
                << ", available resource: "
                << m_computeResource->GetAvailable());

    return true;
}

void
SixGComputeTask::ScheduleStart(Time delay)
{
    Simulator::Schedule(delay, &SixGComputeTask::Start, this);
}

void
SixGComputeTask::ScheduleFinish(Time delay)
{
    Simulator::Schedule(delay, &SixGComputeTask::Finish, this);
}

bool
SixGComputeTask::IsRunning() const
{
    return m_running;
}

} // namespace ns3
