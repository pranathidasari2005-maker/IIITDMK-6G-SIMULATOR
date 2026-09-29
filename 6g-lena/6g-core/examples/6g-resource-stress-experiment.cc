#include "ns3/core-module.h"
#include "ns3/6g-core.h"

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

using namespace ns3;

struct ExperimentResult
{
    uint32_t requests = 0;
    uint32_t successes = 0;
    uint32_t failures = 0;
    uint32_t dynamicMatches = 0;
    double finalNodeAAvailable = 0.0;
    double finalNodeBAvailable = 0.0;
};

static std::vector<SixGCapability> g_capabilities;

static double g_nodeACapacity = 100.0;
static double g_nodeBCapacity = 80.0;
static uint32_t g_requestCount = 5;
static double g_requestStart = 20.0;
static double g_requestStep = 20.0;

static SixGCapability*
FindCapability(const std::string& nodeId)
{
    for (auto& capability : g_capabilities)
    {
        if (capability.nodeId == nodeId)
        {
            return &capability;
        }
    }
    return nullptr;
}

static void
Consume(const std::string& nodeId, double amount)
{
    auto* capability = FindCapability(nodeId);

    if (capability != nullptr)
    {
        capability->availableResource =
            std::max(0.0, capability->availableResource - amount);

        capability->available =
            capability->availableResource > 0.0;

        capability->lastUpdated = Simulator::Now();
    }
}

static void
Release(const std::string& nodeId, double amount)
{
    auto* capability = FindCapability(nodeId);

    if (capability != nullptr)
    {
        capability->availableResource =
            std::min(capability->capacity,
                     capability->availableResource + amount);

        capability->available =
            capability->availableResource > 0.0;

        capability->lastUpdated = Simulator::Now();
    }
}

static bool
StaticSelection(double requiredCompute,
                std::string& selectedNode)
{
    selectedNode = "";

    double highestCapacity = -1.0;

    for (const auto& capability : g_capabilities)
    {
        if (capability.type == "COMPUTING" &&
            capability.capacity > highestCapacity)
        {
            highestCapacity = capability.capacity;
            selectedNode = capability.nodeId;
        }
    }

    auto* selected = FindCapability(selectedNode);

    if (selected == nullptr ||
        !selected->available ||
        selected->availableResource < requiredCompute)
    {
        return false;
    }

    return true;
}

static bool
DynamicSelection(double requiredCompute,
                 std::string& selectedNode)
{
    selectedNode = "";

    SixGCore core;
    SixGCapabilityRequest request;

    request.type = "COMPUTING";
    request.minimumResource = requiredCompute;
    request.requireAvailable = true;

    std::vector<std::string> candidates;

    for (const auto& capability : g_capabilities)
    {
        core.RegisterCapability(capability);
        candidates.push_back(capability.nodeId);
    }

    auto matches = core.DiscoverCapabilities(request, candidates);

    if (matches.empty())
    {
        return false;
    }

    double bestAvailable = -1.0;

    for (const auto& match : matches)
    {
        if (match.availableResource > bestAvailable)
        {
            bestAvailable = match.availableResource;
            selectedNode = match.nodeId;
        }
    }

    return !selectedNode.empty();
}

static void
PrintRequest(uint32_t id,
             double required,
             const std::string& node,
             bool success,
             const std::string& mode)
{
    std::cout << "Request " << id
              << " | mode=" << mode
              << " | required=" << required
              << " | selected=" << (node.empty() ? "NONE" : node)
              << " | result=" << (success ? "SUCCESS" : "FAILURE")
              << std::endl;
}

static ExperimentResult
RunStaticExperiment()
{
    ExperimentResult result;

    g_capabilities.clear();

    SixGCapability nodeA;
    nodeA.nodeId = "EdgeNode-1";
    nodeA.type = "COMPUTING";
    nodeA.capacity = g_nodeACapacity;
    nodeA.availableResource = g_nodeACapacity;
    nodeA.available = true;

    SixGCapability nodeB;
    nodeB.nodeId = "EdgeNode-2";
    nodeB.type = "COMPUTING";
    nodeB.capacity = g_nodeBCapacity;
    nodeB.availableResource = g_nodeBCapacity;
    nodeB.available = true;

    g_capabilities.push_back(nodeA);
    g_capabilities.push_back(nodeB);

    std::cout << "\n========================================\n";
    std::cout << "STATIC SELECTION - INCREASING COMPUTE DEMAND\n";
    std::cout << "========================================\n";

    for (uint32_t i = 0; i < g_requestCount; ++i)
    {
        double required = g_requestStart + (i * g_requestStep);
        result.requests++;

        std::string selected;
        bool success = StaticSelection(required, selected);

        PrintRequest(i + 1,
                     required,
                     selected,
                     success,
                     "STATIC");

        if (success)
        {
            result.successes++;
            Consume(selected, required);
        }
        else
        {
            result.failures++;
        }
    }

    result.finalNodeAAvailable =
        FindCapability("EdgeNode-1")->availableResource;

    result.finalNodeBAvailable =
        FindCapability("EdgeNode-2")->availableResource;

    return result;
}

static ExperimentResult
RunDynamicExperiment()
{
    ExperimentResult result;

    g_capabilities.clear();

    SixGCapability nodeA;
    nodeA.nodeId = "EdgeNode-1";
    nodeA.type = "COMPUTING";
    nodeA.capacity = g_nodeACapacity;
    nodeA.availableResource = g_nodeACapacity;
    nodeA.available = true;

    SixGCapability nodeB;
    nodeB.nodeId = "EdgeNode-2";
    nodeB.type = "COMPUTING";
    nodeB.capacity = g_nodeBCapacity;
    nodeB.availableResource = g_nodeBCapacity;
    nodeB.available = true;

    g_capabilities.push_back(nodeA);
    g_capabilities.push_back(nodeB);

    std::cout << "\n========================================\n";
    std::cout << "DYNAMIC CAPABILITY DISCOVERY - INCREASING COMPUTE DEMAND\n";
    std::cout << "========================================\n";

    for (uint32_t i = 0; i < g_requestCount; ++i)
    {
        double required = g_requestStart + (i * g_requestStep);
        result.requests++;

        std::string selected;
        bool success = DynamicSelection(required, selected);

        if (!selected.empty())
        {
            result.dynamicMatches++;
        }

        PrintRequest(i + 1,
                     required,
                     selected,
                     success,
                     "DYNAMIC");

        if (success)
        {
            result.successes++;
            Consume(selected, required);
        }
        else
        {
            result.failures++;
        }

    }

    result.finalNodeAAvailable =
        FindCapability("EdgeNode-1")->availableResource;

    result.finalNodeBAvailable =
        FindCapability("EdgeNode-2")->availableResource;

    return result;
}

int
main(int argc, char* argv[])
{
    CommandLine cmd(__FILE__);

    cmd.AddValue("nodeACapacity",
                 "Initial compute capacity of EdgeNode-1",
                 g_nodeACapacity);

    cmd.AddValue("nodeBCapacity",
                 "Initial compute capacity of EdgeNode-2",
                 g_nodeBCapacity);

    cmd.AddValue("requestCount",
                 "Number of workload requests",
                 g_requestCount);

    cmd.AddValue("requestStart",
                 "Compute demand of the first request",
                 g_requestStart);

    cmd.AddValue("requestStep",
                 "Increase in compute demand between requests",
                 g_requestStep);

    cmd.Parse(argc, argv);

    std::cout << "\n============================================\n";
    std::cout << "     6G-LENA EXPERIMENT 2\n";
    std::cout << " Dynamic Capability Discovery Under Increasing Workload\n";
    std::cout << "============================================\n";

    ExperimentResult staticResult = RunStaticExperiment();
    ExperimentResult dynamicResult = RunDynamicExperiment();

    double staticSuccessRate =
        100.0 * staticResult.successes / staticResult.requests;

    double dynamicSuccessRate =
        100.0 * dynamicResult.successes / dynamicResult.requests;

    std::cout << "\n========================================\n";
    std::cout << "EXPERIMENT RESULTS\n";
    std::cout << "========================================\n";

    std::cout << std::fixed << std::setprecision(2);

    std::cout << "\nSTATIC SELECTION\n";
    std::cout << "Requests           : " << staticResult.requests << "\n";
    std::cout << "Successful         : " << staticResult.successes << "\n";
    std::cout << "Failed             : " << staticResult.failures << "\n";
    std::cout << "Success rate       : " << staticSuccessRate << "%\n";
    std::cout << "Final EdgeNode-1   : "
              << staticResult.finalNodeAAvailable << "\n";
    std::cout << "Final EdgeNode-2   : "
              << staticResult.finalNodeBAvailable << "\n";

    std::cout << "\nDYNAMIC CAPABILITY DISCOVERY\n";
    std::cout << "Requests           : " << dynamicResult.requests << "\n";
    std::cout << "Successful         : " << dynamicResult.successes << "\n";
    std::cout << "Failed             : " << dynamicResult.failures << "\n";
    std::cout << "Discovery matches  : " << dynamicResult.dynamicMatches << "\n";
    std::cout << "Success rate       : " << dynamicSuccessRate << "%\n";
    std::cout << "Final EdgeNode-1   : "
              << dynamicResult.finalNodeAAvailable << "\n";
    std::cout << "Final EdgeNode-2   : "
              << dynamicResult.finalNodeBAvailable << "\n";

    double improvement =
        dynamicSuccessRate - staticSuccessRate;

    std::cout << "\n----------------------------------------\n";
    std::cout << "Dynamic success-rate improvement: "
              << improvement << " percentage points\n";
    std::cout << "----------------------------------------\n";

    std::cout << "\n=== Experiment completed ===\n";

    return 0;
}
