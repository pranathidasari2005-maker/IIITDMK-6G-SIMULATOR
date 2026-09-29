#include "ns3/core-module.h"
#include "ns3/6g-core.h"

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

using namespace ns3;

struct FreshnessResult
{
    uint32_t requests = 0;
    uint32_t expectedMatches = 0;
    uint32_t actualMatches = 0;
    uint32_t correctResults = 0;
};

static SixGCore g_core;

static double g_nodeAValidity = 5.0;
static double g_nodeBValidity = 10.0;
static double g_minimumResource = 20.0;
static double g_secondCheckpoint = 6.0;
static double g_thirdCheckpoint = 11.0;

static std::vector<std::string>
GetNodeIds(const std::vector<SixGCapability>& capabilities)
{
    std::vector<std::string> ids;

    for (const auto& capability : capabilities)
    {
        ids.push_back(capability.nodeId);
    }

    return ids;
}

static bool
ContainsNode(const std::vector<SixGCapability>& matches,
             const std::string& nodeId)
{
    for (const auto& match : matches)
    {
        if (match.nodeId == nodeId)
        {
            return true;
        }
    }

    return false;
}

static bool
ValidateMatches(const std::vector<SixGCapability>& matches,
                 const std::vector<std::string>& expected)
{
    if (matches.size() != expected.size())
    {
        return false;
    }

    for (const auto& nodeId : expected)
    {
        if (!ContainsNode(matches, nodeId))
        {
            return false;
        }
    }

    return true;
}

static void
PrintMatches(const std::vector<SixGCapability>& matches)
{
    if (matches.empty())
    {
        std::cout << "NONE";
        return;
    }

    for (size_t i = 0; i < matches.size(); ++i)
    {
        std::cout << matches[i].nodeId;

        if (i + 1 < matches.size())
        {
            std::cout << ", ";
        }
    }
}

static FreshnessResult
RunFreshnessRequest(uint32_t requestId,
                    bool requireFresh,
                    const std::vector<std::string>& expected)
{
    FreshnessResult result;

    result.requests = 1;
    result.expectedMatches = expected.size();

    SixGCapabilityRequest request;
    request.type = "COMPUTING";
    request.minimumResource = g_minimumResource;
    request.requireAvailable = true;
    request.requireFresh = requireFresh;

    const auto matches = g_core.DiscoverCapabilities(request);

    result.actualMatches = matches.size();

    const bool correct =
        ValidateMatches(matches, expected);

    if (correct)
    {
        result.correctResults = 1;
    }

    std::cout << "\nRequest " << requestId
              << " | t=" << Simulator::Now().GetSeconds()
              << "s"
              << " | requireFresh="
              << (requireFresh ? "TRUE" : "FALSE")
              << std::endl;

    std::cout << "Discovered providers : ";
    PrintMatches(matches);
    std::cout << std::endl;

    std::cout << "Expected providers   : ";

    if (expected.empty())
    {
        std::cout << "NONE";
    }
    else
    {
        for (size_t i = 0; i < expected.size(); ++i)
        {
            std::cout << expected[i];

            if (i + 1 < expected.size())
            {
                std::cout << ", ";
            }
        }
    }

    std::cout << std::endl;

    std::cout << "Expected count       : "
              << expected.size() << std::endl;

    std::cout << "Actual count         : "
              << matches.size() << std::endl;

    std::cout << "Discovery correctness: "
              << (correct ? "PASS" : "FAIL")
              << std::endl;

    return result;
}

static void
PrintCapabilityState()
{
    const auto capabilities = g_core.GetCapabilities();

    std::cout << "\nCapability state at t="
              << Simulator::Now().GetSeconds()
              << "s" << std::endl;

    for (const auto& capability : capabilities)
    {
        const bool fresh =
            g_core.IsCapabilityFresh(capability);

        const double age =
            (Simulator::Now() - capability.lastUpdated).GetSeconds();

        std::cout << capability.nodeId
                  << " | lastUpdated="
                  << capability.lastUpdated.GetSeconds()
                  << "s"
                  << " | age="
                  << age
                  << "s"
                  << " | validity="
                  << capability.validityDuration.GetSeconds()
                  << "s"
                  << " | fresh="
                  << (fresh ? "YES" : "NO")
                  << std::endl;
    }
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
        "secondCheckpoint",
        "Time of the second freshness checkpoint",
        g_secondCheckpoint);

    cmd.AddValue(
        "thirdCheckpoint",
        "Time of the third freshness checkpoint",
        g_thirdCheckpoint);

    cmd.Parse(argc, argv);

    std::cout << "\n============================================\n";
    std::cout << "     6G-LENA EXPERIMENT 3\n";
    std::cout << " Capability Freshness and Temporal Discovery\n";
    std::cout << "============================================\n";

    SixGCapability nodeA;
    nodeA.nodeId = "EdgeNode-1";
    nodeA.type = "COMPUTING";
    nodeA.capacity = 100.0;
    nodeA.availableResource = 100.0;
    nodeA.available = true;
    nodeA.validityDuration = Seconds(g_nodeAValidity);

    SixGCapability nodeB;
    nodeB.nodeId = "EdgeNode-2";
    nodeB.type = "COMPUTING";
    nodeB.capacity = 100.0;
    nodeB.availableResource = 100.0;
    nodeB.available = true;
    nodeB.validityDuration = Seconds(g_nodeBValidity);

    SixGCapability nodeC;
    nodeC.nodeId = "EdgeNode-3";
    nodeC.type = "COMPUTING";
    nodeC.capacity = 100.0;
    nodeC.availableResource = 100.0;
    nodeC.available = true;
    nodeC.validityDuration = Seconds(0.0);

    g_core.RegisterCapability(nodeA);
    g_core.RegisterCapability(nodeB);
    g_core.RegisterCapability(nodeC);

    std::cout << "\nInitial registration completed at t=0s.";
    PrintCapabilityState();

    std::cout << "\n--- t=0s: All capabilities initially fresh ---";

    Simulator::Schedule(
        Seconds(0.0),
        []()
        {
            PrintCapabilityState();

            const auto result =
                RunFreshnessRequest(
                    1,
                    true,
                    {
                        "EdgeNode-1",
                        "EdgeNode-2",
                        "EdgeNode-3"
                    });

            if (result.correctResults != 1)
            {
                NS_FATAL_ERROR(
                    "Freshness discovery failed at t=0s");
            }
        });

    Simulator::Schedule(
        Seconds(g_secondCheckpoint),
        []()
        {
            std::cout << "\n--- t=6s: 5-second capability expired ---";

            PrintCapabilityState();

            const auto freshResult =
                RunFreshnessRequest(
                    2,
                    true,
                    {
                        "EdgeNode-2",
                        "EdgeNode-3"
                    });

            if (freshResult.correctResults != 1)
            {
                NS_FATAL_ERROR(
                    "Freshness filtering failed at t=6s");
            }

            const auto normalResult =
                RunFreshnessRequest(
                    3,
                    false,
                    {
                        "EdgeNode-1",
                        "EdgeNode-2",
                        "EdgeNode-3"
                    });

            if (normalResult.correctResults != 1)
            {
                NS_FATAL_ERROR(
                    "Non-fresh discovery behavior failed at t=6s");
            }
        });

    Simulator::Schedule(
        Seconds(g_thirdCheckpoint),
        []()
        {
            std::cout << "\n--- t=11s: 10-second capability expired ---";

            PrintCapabilityState();

            const auto freshResult =
                RunFreshnessRequest(
                    4,
                    true,
                    {
                        "EdgeNode-3"
                    });

            if (freshResult.correctResults != 1)
            {
                NS_FATAL_ERROR(
                    "Freshness filtering failed at t=11s");
            }

            const auto normalResult =
                RunFreshnessRequest(
                    5,
                    false,
                    {
                        "EdgeNode-1",
                        "EdgeNode-2",
                        "EdgeNode-3"
                    });

            if (normalResult.correctResults != 1)
            {
                NS_FATAL_ERROR(
                    "Non-fresh discovery behavior failed at t=11s");
            }
        });

    Simulator::Run();

    std::cout << "\n============================================\n";
    std::cout << "EXPERIMENT 3 COMPLETED\n";
    std::cout << "============================================\n";

    std::cout << "Temporal checkpoints : 3" << std::endl;
    std::cout << "Freshness behavior   : VERIFIED" << std::endl;
    std::cout << "============================================\n";

    Simulator::Destroy();

    return 0;
}
