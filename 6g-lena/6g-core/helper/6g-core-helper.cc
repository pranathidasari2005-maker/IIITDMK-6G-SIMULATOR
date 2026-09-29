#include "6g-core-helper.h"

namespace ns3
{

Ptr<SixGCore>
SixGCoreHelper::CreateCore(const std::string& scenario) const
{
    Ptr<SixGCore> core = CreateObject<SixGCore>();

    core->SetScenario(scenario);
    core->Initialize();

    return core;
}

} // namespace ns3
