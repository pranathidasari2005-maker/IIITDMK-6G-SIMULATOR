#include "ns3/core-module.h"
#include "ns3/6g-core.h"

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

using namespace ns3;

/*
 * Experiment 1
 *
 * Research Question:
 * Does dynamic capability discovery improve request satisfaction
 * when resource availability changes over time, compared with
 * static selection?
 *
 * Controlled methodology:
 * - Same initial resources
 * - Same request sequence
 * - Same runtime resource update
 * - Same success/failure condition
 * - Only the selection mechanism differs
 */

struct ExperimentResult
{
    uint32_t requests = 0;
    uint32_t successes = 0;
    uint32_t failures = 0;
    uint32_t discoveryMatches = 0;

    double totalRequested = 0.0;
    double totalAllocated = 0.0;

    double finalNodeAAvailable = 0.0;
    double finalNodeBAvailable = 0.0;
};

static std::vector<SixGCapability> g_capabilities;

static uint32_t g_requestCount = 10;
static uint32_t g_workloadProfile = 1;
static bool g_runtimeUpdates = true;

static std::vector<double>
GenerateRequests()
{
    /*
     * Deterministic workload profiles.
     *
     * The first 10 requests define the controlled workload pattern.
     * For larger experiments, the pattern is repeated deterministically.
     * Therefore the same request count and profile always produce
     * exactly the same workload sequence.
     */

    const std::vector<double> workloadProfile1 =
    {
        60.0,
        60.0,
        30.0,
        50.0,
        20.0,
        20.0,
        10.0,
        30.0,
        20.0,
        10.0
    };

    const std::vector<double> workloadProfile2 =
    {
        60.0,
        30.0,
        20.0,
        50.0,
        20.0,
        20.0,
        10.0,
        30.0,
        20.0,
        10.0
    };

    const std::vector<double>& workload =
        (g_workloadProfile == 2)
            ? workloadProfile2
            : workloadProfile1;

    std::vector<double> requests;
    requests.reserve(g_requestCount);

    for (uint32_t i = 0; i < g_requestCount; ++i)
    {
        /*
         * Deterministic cyclic workload generation.
         */
        requests.push_back(workload[i % workload.size()]);
    }

    return requests;
}


/* ------------------------------------------------------------
 * Common scenario setup
 * ------------------------------------------------------------ */

static void
InitializeScenario()
{
    g_capabilities.clear();

    SixGCapability nodeA;
    nodeA.nodeId = "EdgeNode-1";
    nodeA.type = "COMPUTING";
    nodeA.capacity = 100.0;
    nodeA.availableResource = 100.0;
    nodeA.available = true;
    nodeA.state = SixGCapabilityState::ACTIVE;
    nodeA.lastUpdated = Simulator::Now();

    SixGCapability nodeB;
    nodeB.nodeId = "EdgeNode-2";
    nodeB.type = "COMPUTING";
    nodeB.capacity = 80.0;
    nodeB.availableResource = 80.0;
    nodeB.available = true;
    nodeB.state = SixGCapabilityState::ACTIVE;
    nodeB.lastUpdated = Simulator::Now();

    g_capabilities.push_back(nodeA);
    g_capabilities.push_back(nodeB);
}


/* ------------------------------------------------------------
 * Capability lookup
 * ------------------------------------------------------------ */

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


/* ------------------------------------------------------------
 * Runtime resource state
 * ------------------------------------------------------------ */

static void
Consume(const std::string& nodeId, double amount)
{
    auto* capability = FindCapability(nodeId);

    if (capability == nullptr)
    {
        return;
    }

    capability->availableResource =
        std::max(0.0,
                 capability->availableResource - amount);

    capability->available =
        capability->availableResource > 0.0;

    capability->state =
        capability->available
            ? SixGCapabilityState::ACTIVE
            : SixGCapabilityState::UNAVAILABLE;

    capability->lastUpdated = Simulator::Now();
}


static void
Release(const std::string& nodeId, double amount)
{
    auto* capability = FindCapability(nodeId);

    if (capability == nullptr)
    {
        return;
    }

    capability->availableResource =
        std::min(capability->capacity,
                 capability->availableResource + amount);

    capability->available =
        capability->availableResource > 0.0;

    capability->state =
        capability->available
            ? SixGCapabilityState::ACTIVE
            : SixGCapabilityState::UNAVAILABLE;

    capability->lastUpdated = Simulator::Now();
}


/* ------------------------------------------------------------
 * Common runtime event
 *
 * IMPORTANT:
 * This event is applied identically to both experiments.
 * ------------------------------------------------------------ */

static void
ApplyRuntimeUpdate(uint32_t requestIndex)
{
    /*
     * After request 3, EdgeNode-1 releases 40 compute units.
     *
     * Before release:
     *   EdgeNode-1 = 20
     *
     * After release:
     *   EdgeNode-1 = 60
     *
     * This changes the runtime resource state and therefore
     * changes the correct selection decision.
     */

    if (requestIndex == 3)
    {
        Release("EdgeNode-1", 40.0);

        auto* node = FindCapability("EdgeNode-1");

        std::cout << "  Runtime update: EdgeNode-1 released 40 compute"
                  << " | available="
                  << node->availableResource
                  << std::endl;
    }

    if (requestIndex == 5)
    {
        Release("EdgeNode-2", 30.0);

        auto* node = FindCapability("EdgeNode-2");

        std::cout << "  Runtime update: EdgeNode-2 released 30 compute"
                  << " | available="
                  << node->availableResource
                  << std::endl;
    }
}


/* ------------------------------------------------------------
 * Static selection
 *
 * Baseline:
 * Always selects the node with the highest configured capacity.
 * It does NOT discover candidates using the current capability
 * registry.
 * ------------------------------------------------------------ */

static bool
StaticSelection(double requiredCompute,
                std::string& selectedNode)
{
    selectedNode.clear();

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

    if (selected == nullptr)
    {
        return false;
    }

    if (!selected->available)
    {
        return false;
    }

    if (selected->availableResource < requiredCompute)
    {
        return false;
    }

    return true;
}


/* ------------------------------------------------------------
 * Dynamic capability discovery
 *
 * Proposed mechanism:
 * 1. Register current capability state
 * 2. Query SixGCore
 * 3. Filter using current available resource
 * 4. Select the candidate with the highest current availability
 * ------------------------------------------------------------ */

static bool
DynamicSelection(double requiredCompute,
                 std::string& selectedNode,
                 uint32_t& matchCount)
{
    selectedNode.clear();
    matchCount = 0;

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

    auto matches =
        core.DiscoverCapabilities(request, candidates);

    matchCount = static_cast<uint32_t>(matches.size());

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


/* ------------------------------------------------------------
 * Output helper
 * ------------------------------------------------------------ */

static void
PrintRequest(uint32_t id,
             double required,
             const std::string& node,
             bool success,
             const std::string& mode,
             uint32_t matches = 0)
{
    std::cout << "Request " << id
              << " | mode=" << mode
              << " | required=" << required
              << " | selected="
              << (node.empty() ? "NONE" : node);

    if (mode == "DYNAMIC")
    {
        std::cout << " | candidates=" << matches;
    }

    std::cout << " | result="
              << (success ? "SUCCESS" : "FAILURE")
              << std::endl;
}


/* ------------------------------------------------------------
 * Static experiment
 * ------------------------------------------------------------ */

static ExperimentResult
RunStaticExperiment()
{
    ExperimentResult result;

    InitializeScenario();

    std::cout << "\n========================================\n";
    std::cout << "STATIC SELECTION EXPERIMENT\n";
    std::cout << "========================================\n";

    /*
     * This request sequence is the common workload used by
     * BOTH static and dynamic experiments.
     */
    const std::vector<double> requests =
        GenerateRequests();

    for (uint32_t i = 0; i < requests.size(); ++i)
    {
        result.requests++;
        result.totalRequested += requests[i];

        std::string selected;

        bool success =
            StaticSelection(requests[i], selected);

        PrintRequest(i + 1,
                     requests[i],
                     selected,
                     success,
                     "STATIC");

        if (success)
        {
            result.successes++;
            result.totalAllocated += requests[i];

            Consume(selected, requests[i]);
        }
        else
        {
            result.failures++;
        }

        /*
         * SAME runtime update as the dynamic experiment.
         */
        if (g_runtimeUpdates)
        {
            ApplyRuntimeUpdate(i + 1);
        }
    }

    result.finalNodeAAvailable =
        FindCapability("EdgeNode-1")->availableResource;

    result.finalNodeBAvailable =
        FindCapability("EdgeNode-2")->availableResource;

    return result;
}


/* ------------------------------------------------------------
 * Dynamic experiment
 * ------------------------------------------------------------ */

static ExperimentResult
RunDynamicExperiment()
{
    ExperimentResult result;

    InitializeScenario();

    std::cout << "\n========================================\n";
    std::cout << "DYNAMIC CAPABILITY DISCOVERY EXPERIMENT\n";
    std::cout << "========================================\n";

    /*
     * EXACTLY the same workload as the static experiment.
     */
    const std::vector<double> requests =
        GenerateRequests();

    for (uint32_t i = 0; i < requests.size(); ++i)
    {
        result.requests++;
        result.totalRequested += requests[i];

        std::string selected;
        uint32_t matches = 0;

        bool success =
            DynamicSelection(requests[i],
                             selected,
                             matches);

        result.discoveryMatches += matches;

        PrintRequest(i + 1,
                     requests[i],
                     selected,
                     success,
                     "DYNAMIC",
                     matches);

        if (success)
        {
            result.successes++;
            result.totalAllocated += requests[i];

            Consume(selected, requests[i]);
        }
        else
        {
            result.failures++;
        }

        /*
         * EXACTLY the same runtime update as the static experiment.
         */
        if (g_runtimeUpdates)
        {
            ApplyRuntimeUpdate(i + 1);
        }
    }

    result.finalNodeAAvailable =
        FindCapability("EdgeNode-1")->availableResource;

    result.finalNodeBAvailable =
        FindCapability("EdgeNode-2")->availableResource;

    return result;
}


/* ------------------------------------------------------------
 * Main
 * ------------------------------------------------------------ */

int
main(int argc, char* argv[])
{
    CommandLine cmd(__FILE__);

    cmd.AddValue("requests",
                 "Number of deterministic service requests",
                 g_requestCount);

    cmd.AddValue("profile",
                 "Deterministic workload profile (1 or 2)",
                 g_workloadProfile);

    cmd.AddValue("updates",
                 "Enable runtime resource updates (1=ON, 0=OFF)",
                 g_runtimeUpdates);

    cmd.Parse(argc, argv);

    if (g_workloadProfile != 1 && g_workloadProfile != 2)
    {
        std::cerr << "Error: --profile must be 1 or 2."
                  << std::endl;
        return 1;
    }

    if (g_requestCount == 0)
    {
        std::cerr << "Error: --requests must be greater than zero."
                  << std::endl;
        return 1;
    }

    std::cout << "\n============================================\n";
    std::cout << "     6G-LENA EXPERIMENT 1\n";
    std::cout << " Dynamic Capability Discovery vs Static Selection\n";
    std::cout << "============================================\n";
    std::cout << "Workload profile       : "
              << g_workloadProfile << std::endl;
    std::cout << "Runtime updates        : "
              << (g_runtimeUpdates ? "ON" : "OFF") << std::endl;
    std::cout << "Request count          : "
              << g_requestCount << std::endl;

    ExperimentResult staticResult =
        RunStaticExperiment();

    ExperimentResult dynamicResult =
        RunDynamicExperiment();

    double staticSuccessRate =
        100.0 * staticResult.successes /
        staticResult.requests;

    double dynamicSuccessRate =
        100.0 * dynamicResult.successes /
        dynamicResult.requests;

    double staticFailureRate =
        100.0 * staticResult.failures /
        staticResult.requests;

    double dynamicFailureRate =
        100.0 * dynamicResult.failures /
        dynamicResult.requests;

    double staticAllocationRatio =
        100.0 * staticResult.totalAllocated /
        staticResult.totalRequested;

    double dynamicAllocationRatio =
        100.0 * dynamicResult.totalAllocated /
        dynamicResult.totalRequested;

    double successRateImprovement =
        dynamicSuccessRate - staticSuccessRate;

    std::cout << "\n========================================\n";
    std::cout << "EXPERIMENT RESULTS\n";
    std::cout << "========================================\n";

    std::cout << std::fixed
              << std::setprecision(2);

    std::cout << "\nSTATIC SELECTION\n";
    std::cout << "Requests              : "
              << staticResult.requests << "\n";
    std::cout << "Successful            : "
              << staticResult.successes << "\n";
    std::cout << "Failed                : "
              << staticResult.failures << "\n";
    std::cout << "Success rate          : "
              << staticSuccessRate << "%\n";
    std::cout << "Failure rate          : "
              << staticFailureRate << "%\n";
    std::cout << "Requested compute     : "
              << staticResult.totalRequested << "\n";
    std::cout << "Allocated compute     : "
              << staticResult.totalAllocated << "\n";
    std::cout << "Allocation ratio      : "
              << staticAllocationRatio << "%\n";
    std::cout << "Final EdgeNode-1      : "
              << staticResult.finalNodeAAvailable << "\n";
    std::cout << "Final EdgeNode-2      : "
              << staticResult.finalNodeBAvailable << "\n";

    std::cout << "\nDYNAMIC CAPABILITY DISCOVERY\n";
    std::cout << "Requests              : "
              << dynamicResult.requests << "\n";
    std::cout << "Successful            : "
              << dynamicResult.successes << "\n";
    std::cout << "Failed                : "
              << dynamicResult.failures << "\n";
    std::cout << "Discovery candidates  : "
              << dynamicResult.discoveryMatches << "\n";
    std::cout << "Success rate          : "
              << dynamicSuccessRate << "%\n";
    std::cout << "Failure rate          : "
              << dynamicFailureRate << "%\n";
    std::cout << "Requested compute     : "
              << dynamicResult.totalRequested << "\n";
    std::cout << "Allocated compute     : "
              << dynamicResult.totalAllocated << "\n";
    std::cout << "Allocation ratio      : "
              << dynamicAllocationRatio << "%\n";
    std::cout << "Final EdgeNode-1      : "
              << dynamicResult.finalNodeAAvailable << "\n";
    std::cout << "Final EdgeNode-2      : "
              << dynamicResult.finalNodeBAvailable << "\n";

    std::cout << "\n----------------------------------------\n";
    std::cout << "COMPARISON\n";
    std::cout << "----------------------------------------\n";

    std::cout << "Success-rate difference : "
              << successRateImprovement
              << " percentage points\n";

    std::cout << "Allocation-ratio diff   : "
              << dynamicAllocationRatio -
                     staticAllocationRatio
              << " percentage points\n";

    std::cout << "----------------------------------------\n";

    /*
     * Machine-readable experiment output.
     *
     * The CSV contains values generated directly from this
     * completed ns-3 simulation run.
     */
    std::ofstream csv("capability-discovery-results.csv");

    if (!csv.is_open())
    {
        std::cerr << "Warning: Could not create capability-discovery-results.csv"
                  << std::endl;
    }
    else
    {
        csv << "profile,updates,requests,"
            << "static_success_rate,dynamic_success_rate,"
            << "static_allocation_ratio,dynamic_allocation_ratio,"
            << "success_rate_difference,allocation_ratio_difference,"
            << "discovery_candidates\n";

        csv << g_workloadProfile << ","
            << (g_runtimeUpdates ? 1 : 0) << ","
            << dynamicResult.requests << ","
            << staticSuccessRate << ","
            << dynamicSuccessRate << ","
            << staticAllocationRatio << ","
            << dynamicAllocationRatio << ","
            << successRateImprovement << ","
            << dynamicAllocationRatio - staticAllocationRatio << ","
            << dynamicResult.discoveryMatches
            << "\n";

        csv.close();

        std::cout << "CSV output             : capability-discovery-results.csv\n";
    }

    std::cout << "\n=== Experiment completed ===\n";

    return 0;
}
