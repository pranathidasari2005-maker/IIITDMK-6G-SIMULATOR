#include "ns3/core-module.h"
#include "ns3/6g-core.h"

#include <chrono>
#include <iostream>
#include <vector>
#include <string>
#include <iomanip>

using namespace ns3;

static void
RunScalabilityCase(uint32_t providerCount, uint32_t repetitions)
{
    Ptr<SixGCore> core = CreateObject<SixGCore>();

    for (uint32_t i = 0; i < providerCount; ++i)
    {
        SixGCapability capability;
        capability.nodeId = "EdgeNode-" + std::to_string(i + 1);
        capability.type = "COMPUTING";
        capability.capacity = 100.0 + (i % 5) * 20.0;
        capability.availableResource = capability.capacity;
        capability.available = true;
        capability.state = SixGCapabilityState::ACTIVE;
        capability.lastUpdated = Simulator::Now();
        capability.validityDuration = Seconds(0);

        core->RegisterCapability(capability);
    }

    SixGCapabilityRequest request;
    request.type = "COMPUTING";
    request.minimumResource = 100.0;
    request.requireAvailable = true;
    request.requireFresh = false;

    // Warm-up discovery.
    core->DiscoverCapabilities(request);

    auto start = std::chrono::steady_clock::now();

    uint64_t totalMatches = 0;

    for (uint32_t i = 0; i < repetitions; ++i)
    {
        std::vector<SixGCapability> matches =
            core->DiscoverCapabilities(request);

        totalMatches += matches.size();
    }

    auto end = std::chrono::steady_clock::now();

    const double elapsedUs =
        std::chrono::duration<double, std::micro>(end - start).count();

    const double averageUs =
        elapsedUs / static_cast<double>(repetitions);

    std::cout
        << providerCount << ","
        << repetitions << ","
        << std::fixed << std::setprecision(3)
        << elapsedUs << ","
        << averageUs << ","
        << totalMatches
        << std::endl;
}

int
main(int argc, char* argv[])
{
    uint32_t repetitions = 1000;

    CommandLine cmd;
    cmd.AddValue(
        "repetitions",
        "Number of repeated discovery operations per case",
        repetitions);
    cmd.Parse(argc, argv);

    std::cout << "6G-LENA Capability Discovery Scalability Experiment"
              << std::endl;
    std::cout << "Research question: discovery behavior as provider "
                 "population increases"
              << std::endl;
    std::cout << std::endl;

    std::cout
        << "provider_count,"
        << "repetitions,"
        << "total_elapsed_us,"
        << "average_discovery_us,"
        << "total_matches"
        << std::endl;

    const std::vector<uint32_t> providerCounts =
        {10, 50, 100, 250, 500};

    for (uint32_t count : providerCounts)
    {
        RunScalabilityCase(count, repetitions);
    }

    std::cout << std::endl;
    std::cout << "Experiment completed." << std::endl;

    return 0;
}
