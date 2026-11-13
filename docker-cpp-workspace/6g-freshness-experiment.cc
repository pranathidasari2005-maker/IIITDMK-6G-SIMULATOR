#include "ns3/core-module.h"
#include "ns3/6g-core.h"

#include <iostream>
#include <iomanip>

using namespace ns3;

static SixGCore g_core;

static double g_nodeAValidity = 4.0;
static double g_nodeBValidity = 10.0;
static double g_minimumResource = 50.0;
static double g_checkTime = 5.0;

static void
RunFreshnessCheck(const std::string& label)
{
    SixGCapabilityRequest normalRequest;
    normalRequest.type = "COMPUTING";
    normalRequest.minimumResource = g_minimumResource;
    normalRequest.requireAvailable = true;
    normalRequest.requireFresh = false;

    SixGCapabilityRequest freshRequest = normalRequest;
    freshRequest.requireFresh = true;

    auto normalMatches =
        g_core.DiscoverCapabilities(normalRequest);

    auto freshMatches =
        g_core.DiscoverCapabilities(freshRequest);

    std::cout << "\n[" << label << "] Simulation time: "
              << Simulator::Now().GetSeconds() << " s\n";

    std::cout << "Normal discovery (requireFresh=false): "
              << normalMatches.size() << " matches\n";

    for (const auto& capability : normalMatches)
    {
        std::cout << "  " << capability.nodeId
                  << " | lastUpdated="
                  << capability.lastUpdated.GetSeconds()
                  << "s | validity="
                  << capability.validityDuration.GetSeconds()
                  << "s\n";
    }

    std::cout << "Fresh-only discovery (requireFresh=true): "
              << freshMatches.size() << " matches\n";

    for (const auto& capability : freshMatches)
    {
        std::cout << "  " << capability.nodeId
                  << " | FRESH\n";
    }
}

static void
InitializeCapabilities()
{
    SixGCapability nodeA;
    nodeA.nodeId = "EdgeNode-1";
    nodeA.type = "COMPUTING";
    nodeA.capacity = 100.0;
    nodeA.availableResource = 100.0;
    nodeA.available = true;
    nodeA.lastUpdated = Seconds(0);
    nodeA.validityDuration = Seconds(g_nodeAValidity);

    SixGCapability nodeB;
    nodeB.nodeId = "EdgeNode-2";
    nodeB.type = "COMPUTING";
    nodeB.capacity = 80.0;
    nodeB.availableResource = 80.0;
    nodeB.available = true;
    nodeB.lastUpdated = Seconds(0);
    nodeB.validityDuration = Seconds(g_nodeBValidity);

    g_core.RegisterCapability(nodeA);
    g_core.RegisterCapability(nodeB);

    std::cout << "Capabilities registered:\n";
    std::cout << "  EdgeNode-1 | validity="
              << g_nodeAValidity << "s\n";
    std::cout << "  EdgeNode-2 | validity="
              << g_nodeBValidity << "s\n";
}

int
main(int argc, char* argv[])
{
    CommandLine cmd(__FILE__);

    cmd.AddValue(
        "nodeAValidity",
        "EdgeNode-1 capability validity duration in seconds",
        g_nodeAValidity);

    cmd.AddValue(
        "nodeBValidity",
        "EdgeNode-2 capability validity duration in seconds",
        g_nodeBValidity);

    cmd.AddValue(
        "minimumResource",
        "Minimum computing resource required for discovery",
        g_minimumResource);

    cmd.AddValue(
        "checkTime",
        "Time in seconds for the second freshness check",
        g_checkTime);

    cmd.Parse(argc, argv);

    std::cout << "\n============================================\n";
    std::cout << "     6G-LENA EXPERIMENT 3\n";
    std::cout << "       Capability Freshness\n";
    std::cout << "============================================\n";

    InitializeCapabilities();

    Simulator::Schedule(Seconds(0),
                         &RunFreshnessCheck,
                         "Initial discovery");

    Simulator::Schedule(Seconds(g_checkTime),
                         &RunFreshnessCheck,
                         "After capability aging");

    Simulator::Run();
    Simulator::Destroy();

    std::cout << "\n============================================\n";
    std::cout << "Freshness experiment completed\n";
    std::cout << "============================================\n";

    return 0;
}
