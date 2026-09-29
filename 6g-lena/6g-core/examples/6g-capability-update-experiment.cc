#include "ns3/core-module.h"
#include "ns3/6g-core.h"

#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

using namespace ns3;

static SixGCapability
CreateCapability(const std::string& nodeId,
                 double capacity,
                 double available)
{
    SixGCapability capability;

    capability.nodeId = nodeId;
    capability.type = "COMPUTING";
    capability.capacity = capacity;
    capability.availableResource = available;
    capability.available = available > 0.0;
    capability.state =
        capability.available
            ? SixGCapabilityState::ACTIVE
            : SixGCapabilityState::UNAVAILABLE;

    capability.lastUpdated = Simulator::Now();
    capability.validityDuration = Seconds(0);

    return capability;
}

static void
PrintDiscovery(Ptr<SixGCore> core,
               const std::string& label,
               double minimumResource)
{
    SixGCapabilityRequest request;
    request.type = "COMPUTING";
    request.minimumResource = minimumResource;
    request.requireAvailable = true;

    std::vector<SixGCapability> matches =
        core->DiscoverCapabilities(request);

    std::cout
        << std::fixed << std::setprecision(2)
        << "time=" << Simulator::Now().GetSeconds()
        << ",state=" << label
        << ",minimum_resource=" << minimumResource
        << ",matches=" << matches.size();

    for (const auto& capability : matches)
    {
        std::cout
            << ",provider=" << capability.nodeId
            << ",available_resource="
            << capability.availableResource;
    }

    std::cout << std::endl;
}

int
main(int argc, char* argv[])
{
    CommandLine cmd(__FILE__);
    cmd.Parse(argc, argv);

    Ptr<SixGCore> core =
        CreateObject<SixGCore>();

    const double minimumResource = 60.0;

    SixGCapability edgeNode1 =
        CreateCapability("EdgeNode-1", 100.0, 100.0);

    SixGCapability edgeNode2 =
        CreateCapability("EdgeNode-2", 80.0, 80.0);

    core->RegisterCapability(edgeNode1);
    core->RegisterCapability(edgeNode2);

    std::cout
        << "6G-LENA Dynamic Capability Update Experiment"
        << std::endl;

    std::cout
        << "research_question=Can capability discovery adapt "
           "to runtime provider-state changes?"
        << std::endl;

    std::cout
        << "minimum_resource="
        << minimumResource
        << std::endl;

    std::cout << std::endl;

    PrintDiscovery(
        core,
        "INITIAL",
        minimumResource);

    /*
     * At t=1, EdgeNode-1 becomes resource constrained.
     */
    Simulator::Schedule(
        Seconds(1.0),
        [core, minimumResource]()
        {
            SixGCapability updated =
                CreateCapability(
                    "EdgeNode-1",
                    100.0,
                    40.0);

            core->UpdateCapability(updated);

            PrintDiscovery(
                core,
                "EDGE_NODE_1_DEGRADED",
                minimumResource);
        });

    /*
     * At t=2, EdgeNode-2 also becomes resource constrained.
     */
    Simulator::Schedule(
        Seconds(2.0),
        [core, minimumResource]()
        {
            SixGCapability updated =
                CreateCapability(
                    "EdgeNode-2",
                    80.0,
                    30.0);

            core->UpdateCapability(updated);

            PrintDiscovery(
                core,
                "BOTH_PROVIDERS_BELOW_THRESHOLD",
                minimumResource);
        });

    /*
     * At t=3, EdgeNode-1 recovers.
     */
    Simulator::Schedule(
        Seconds(3.0),
        [core, minimumResource]()
        {
            SixGCapability updated =
                CreateCapability(
                    "EdgeNode-1",
                    100.0,
                    90.0);

            core->UpdateCapability(updated);

            PrintDiscovery(
                core,
                "EDGE_NODE_1_RECOVERED",
                minimumResource);
        });

    /*
     * At t=4, EdgeNode-2 recovers with more available
     * resource than EdgeNode-1.
     */
    Simulator::Schedule(
        Seconds(4.0),
        [core, minimumResource]()
        {
            SixGCapability updated =
                CreateCapability(
                    "EdgeNode-2",
                    80.0,
                    100.0);

            core->UpdateCapability(updated);

            PrintDiscovery(
                core,
                "EDGE_NODE_2_RECOVERED",
                minimumResource);
        });

    Simulator::Stop(Seconds(5.0));
    Simulator::Run();
    Simulator::Destroy();

    std::cout << std::endl;
    std::cout
        << "Experiment completed."
        << std::endl;

    return 0;
}
