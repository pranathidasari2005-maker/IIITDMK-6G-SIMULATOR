#include "ns3/core-module.h"
#include "ns3/6g-core.h"

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

using namespace ns3;

struct ConstraintExperimentResult
{
    uint32_t requests = 0;
    uint32_t successfulDiscoveries = 0;
    uint32_t totalMatches = 0;
    uint32_t correctDiscoveries = 0;
};

static std::vector<SixGCapability> g_capabilities;

static void
RegisterScenarioCapabilities(SixGCore& core)
{
    g_capabilities.clear();

    SixGCapability edge1;
    edge1.nodeId = "EdgeNode-1";
    edge1.type = "COMPUTING";
    edge1.capacity = 100.0;
    edge1.availableResource = 100.0;
    edge1.available = true;
    edge1.attributes["latencyClass"] = "LOW";
    edge1.attributes["reliabilityClass"] = "HIGH";

    SixGCapability edge2;
    edge2.nodeId = "EdgeNode-2";
    edge2.type = "COMPUTING";
    edge2.capacity = 80.0;
    edge2.availableResource = 80.0;
    edge2.available = true;
    edge2.attributes["latencyClass"] = "MEDIUM";
    edge2.attributes["reliabilityClass"] = "HIGH";

    SixGCapability edge3;
    edge3.nodeId = "EdgeNode-3";
    edge3.type = "COMPUTING";
    edge3.capacity = 120.0;
    edge3.availableResource = 120.0;
    edge3.available = true;
    edge3.attributes["latencyClass"] = "LOW";
    edge3.attributes["reliabilityClass"] = "MEDIUM";

    SixGCapability edge4;
    edge4.nodeId = "EdgeNode-4";
    edge4.type = "COMPUTING";
    edge4.capacity = 60.0;
    edge4.availableResource = 60.0;
    edge4.available = true;
    edge4.attributes["latencyClass"] = "HIGH";
    edge4.attributes["reliabilityClass"] = "LOW";

    SixGCapability edge5;
    edge5.nodeId = "EdgeNode-5";
    edge5.type = "COMPUTING";
    edge5.capacity = 90.0;
    edge5.availableResource = 90.0;
    edge5.available = true;
    edge5.attributes["latencyClass"] = "LOW";
    edge5.attributes["reliabilityClass"] = "HIGH";

    g_capabilities =
    {
        edge1,
        edge2,
        edge3,
        edge4,
        edge5
    };

    for (const auto& capability : g_capabilities)
    {
        core.RegisterCapability(capability);
    }
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
ValidateDiscovery(const std::vector<SixGCapability>& matches,
                  const std::vector<std::string>& expectedNodes)
{
    if (matches.size() != expectedNodes.size())
    {
        return false;
    }

    for (const auto& expectedNode : expectedNodes)
    {
        if (!ContainsNode(matches, expectedNode))
        {
            return false;
        }
    }

    return true;
}

static void
PrintDiscoveryResult(uint32_t requestId,
                     const std::string& description,
                     const std::vector<SixGCapability>& matches,
                     const std::vector<std::string>& expectedNodes,
                     bool correct)
{
    std::cout << "\nRequest " << requestId
              << " | " << description << std::endl;

    std::cout << "Discovered providers : "
              << matches.size() << std::endl;

    std::cout << "Providers            : ";

    if (matches.empty())
    {
        std::cout << "NONE";
    }
    else
    {
        for (size_t i = 0; i < matches.size(); ++i)
        {
            std::cout << matches[i].nodeId;

            if (i + 1 < matches.size())
            {
                std::cout << ", ";
            }
        }
    }

    std::cout << std::endl;

    std::cout << "Expected providers   : ";

    if (expectedNodes.empty())
    {
        std::cout << "NONE";
    }
    else
    {
        for (size_t i = 0; i < expectedNodes.size(); ++i)
        {
            std::cout << expectedNodes[i];

            if (i + 1 < expectedNodes.size())
            {
                std::cout << ", ";
            }
        }
    }

    std::cout << std::endl;

    std::cout << "Discovery correctness: "
              << (correct ? "PASS" : "FAIL")
              << std::endl;
}

static ConstraintExperimentResult
RunConstraintDiscoveryExperiment(double minimumResource)
{
    ConstraintExperimentResult result;

    SixGCore core;
    RegisterScenarioCapabilities(core);

    std::cout << "\n============================================\n";
    std::cout << "     6G-LENA EXPERIMENT 2\n";
    std::cout << " Constraint-Aware Capability Discovery\n";
    std::cout << "============================================\n";

    struct TestCase
    {
        std::string description;
        double minimumResource;
        std::vector<std::string> requiredLatency;
        std::vector<std::string> requiredReliability;
        std::vector<std::string> expectedNodes;
    };

    struct ConstraintCase
    {
        std::string description;
        std::vector<std::string> requiredLatency;
        std::vector<std::string> requiredReliability;
    };

    const std::vector<ConstraintCase> constraintCases =
    {
        {
            "",
            {},
            {}
        },
        {
            " AND latencyClass = LOW",
            {"LOW"},
            {}
        },
        {
            " AND reliabilityClass = HIGH",
            {},
            {"HIGH"}
        },
        {
            " AND latencyClass = LOW AND reliabilityClass = HIGH",
            {"LOW"},
            {"HIGH"}
        }
    };

    std::vector<TestCase> tests;

    for (const auto& constraintCase : constraintCases)
    {
        TestCase test;
        test.description =
            "Compute >= " + std::to_string(static_cast<int>(minimumResource)) +
            constraintCase.description;

        test.minimumResource = minimumResource;
        test.requiredLatency = constraintCase.requiredLatency;
        test.requiredReliability = constraintCase.requiredReliability;

        for (const auto& capability : g_capabilities)
        {
            if (capability.type != "COMPUTING" ||
                !capability.available ||
                capability.availableResource < minimumResource)
            {
                continue;
            }

            bool matches = true;

            if (!test.requiredLatency.empty())
            {
                auto it = capability.attributes.find("latencyClass");

                if (it == capability.attributes.end() ||
                    it->second != test.requiredLatency.front())
                {
                    matches = false;
                }
            }

            if (!test.requiredReliability.empty())
            {
                auto it = capability.attributes.find("reliabilityClass");

                if (it == capability.attributes.end() ||
                    it->second != test.requiredReliability.front())
                {
                    matches = false;
                }
            }

            if (matches)
            {
                test.expectedNodes.push_back(capability.nodeId);
            }
        }

        tests.push_back(test);
    }

    for (uint32_t i = 0; i < tests.size(); ++i)
    {
        const auto& test = tests[i];

        result.requests++;

        SixGCapabilityRequest request;
        request.type = "COMPUTING";
        request.minimumResource = test.minimumResource;
        request.requireAvailable = true;

        if (!test.requiredLatency.empty())
        {
            request.requiredAttributes["latencyClass"] =
                test.requiredLatency.front();
        }

        if (!test.requiredReliability.empty())
        {
            request.requiredAttributes["reliabilityClass"] =
                test.requiredReliability.front();
        }

        const std::vector<std::string> candidateNodeIds =
        {
            "EdgeNode-1",
            "EdgeNode-2",
            "EdgeNode-3",
            "EdgeNode-4",
            "EdgeNode-5"
        };

        auto matches =
            core.DiscoverCapabilities(request, candidateNodeIds);

        result.totalMatches += matches.size();

        if (!matches.empty())
        {
            result.successfulDiscoveries++;
        }

        const bool correct =
            ValidateDiscovery(matches, test.expectedNodes);

        if (correct)
        {
            result.correctDiscoveries++;
        }

        PrintDiscoveryResult(i + 1,
                             test.description,
                             matches,
                             test.expectedNodes,
                             correct);
    }

    return result;
}

int
main(int argc, char* argv[])
{
    double minimumResource = 80.0;

    CommandLine cmd(__FILE__);
    cmd.AddValue(
        "minimum-resource",
        "Minimum computing resource required for capability discovery",
        minimumResource);
    cmd.Parse(argc, argv);

    const ConstraintExperimentResult result =
        RunConstraintDiscoveryExperiment(minimumResource);

    const double correctness =
        result.requests == 0
            ? 0.0
            : 100.0 * static_cast<double>(result.correctDiscoveries) /
                  static_cast<double>(result.requests);

    std::cout << "\n============================================\n";
    std::cout << "EXPERIMENT 2 SUMMARY\n";
    std::cout << "============================================\n";

    std::cout << "Discovery requests    : "
              << result.requests << std::endl;

    std::cout << "Successful discoveries: "
              << result.successfulDiscoveries << std::endl;

    std::cout << "Total provider matches: "
              << result.totalMatches << std::endl;

    std::cout << "Correct discoveries   : "
              << result.correctDiscoveries << std::endl;

    std::cout << std::fixed << std::setprecision(2);

    std::cout << "Correctness rate      : "
              << correctness << "%" << std::endl;

    std::cout << "============================================\n";

    return 0;
}
