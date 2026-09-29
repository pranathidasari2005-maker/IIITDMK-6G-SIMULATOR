#include "ns3/6g-access-node.h"
#include "ns3/6g-edge-node.h"
#include "ns3/6g-capability-matcher.h"
#include "ns3/6g-capability-provider.h"
#include "ns3/6g-compute-task.h"
#include "ns3/6g-core.h"
#include "ns3/6g-ue.h"
#include "ns3/6g-service.h"
#include "ns3/6g-service-session.h"
#include "ns3/6g-core-module.h"
#include "ns3/command-line.h"
#include "ns3/simulator.h"

#include <iostream>
#include <string>
#include <vector>

using namespace ns3;

Ptr<SixGCore> g_core;
Ptr<SixGCapabilityProvider> g_providerA;
Ptr<SixGCapabilityProvider> g_providerB;
Ptr<SixGCapabilityMatcher> g_matcher;
Ptr<SixGComputeTask> g_task;
Ptr<SixGAccessNode> g_accessNode;
Ptr<SixGEdgeNode> g_edgeNode1;
Ptr<SixGEdgeNode> g_edgeNode2;
Ptr<SixGUe> g_ue;
Ptr<SixGUe> g_ue2;
Ptr<SixGService> g_service;
Ptr<SixGService> g_service2;
Ptr<SixGServiceSession> g_session;
Ptr<SixGServiceSession> g_session2;
void
BuildNetworkTopology()
{
    std::cout << "[t=" << Simulator::Now().GetSeconds() << "s] Building network topology" << std::endl;
    g_accessNode = CreateObject<SixGAccessNode>();
    g_accessNode->SetNodeId("AccessNode-1");
    g_accessNode->SetActive(true);
    g_accessNode->SetCommunicationCapacity(100.0);
    g_accessNode->SetAvailableCommunication(100.0);
    g_edgeNode1 = CreateObject<SixGEdgeNode>();
    g_edgeNode1->SetNodeId("EdgeNode-1");
    g_edgeNode1->SetActive(true);
    g_edgeNode2 = CreateObject<SixGEdgeNode>();
    g_edgeNode2->SetNodeId("EdgeNode-2");
    g_edgeNode2->SetActive(true);
    g_ue = CreateObject<SixGUe>();
    g_ue->SetNodeId("UE-1");
    g_ue->SetActive(true);

    g_ue2 = CreateObject<SixGUe>();
    g_ue2->SetNodeId("UE-2");
    g_ue2->SetActive(true);

    g_service = CreateObject<SixGService>();
    g_service->SetServiceId("Service-1");
    g_service->SetRequiredCompute(60.0);
    g_service->SetRequiredCommunication(20.0);
    g_service->SetMaximumLatency(10.0);
    g_service->SetMinimumReliability(0.99);
    g_service->SetActive(true);

    g_service2 = CreateObject<SixGService>();
    g_service2->SetServiceId("Service-2");
    g_service2->SetRequiredCompute(60.0);
    g_service2->SetRequiredCommunication(20.0);
    g_service2->SetMaximumLatency(10.0);
    g_service2->SetMinimumReliability(0.99);
    g_service2->SetActive(true);

    g_accessNode->AddReachableEdgeNode(g_edgeNode1);
    g_accessNode->AddReachableEdgeNode(g_edgeNode2);
    g_ue->SetConnectedAccessNode(g_accessNode);
    g_ue2->SetConnectedAccessNode(g_accessNode);

    g_session = CreateObject<SixGServiceSession>();
    g_session->SetSessionId("Session-1");
    g_session->SetUe(g_ue);
    g_session->SetAccessNode(g_accessNode);
    g_session->SetService(g_service);

    g_session2 = CreateObject<SixGServiceSession>();
    g_session2->SetSessionId("Session-2");
    g_session2->SetUe(g_ue2);
    g_session2->SetAccessNode(g_accessNode);
    g_session2->SetService(g_service2);
    std::cout << "    Access node: " << g_accessNode->GetNodeId() << std::endl;
    std::cout << "    Reachable edge nodes: " << g_accessNode->GetReachableEdgeNodeCount() << std::endl;
    std::cout << "    EdgeNode-1 reachable: " << (g_accessNode->HasReachableEdgeNode(g_edgeNode1) ? "YES" : "NO") << std::endl;
    std::cout << "    EdgeNode-2 reachable: " << (g_accessNode->HasReachableEdgeNode(g_edgeNode2) ? "YES" : "NO") << std::endl;
    std::cout << std::endl;
}

void
RegisterNodes()
{
    std::cout << "[t=" << Simulator::Now().GetSeconds()
              << "s] Capability registration" << std::endl;

    g_providerA = CreateObject<SixGCapabilityProvider>();
    g_providerA->SetCore(g_core);
    g_providerA->SetNodeId("EdgeNode-1");
    g_providerA->SetCapabilityType("COMPUTING");
    g_edgeNode1->SetComputeCapacity(100.0); g_edgeNode1->SetAvailableCompute(100.0); g_providerA->SetResource(g_edgeNode1->GetResource("COMPUTING"));
    g_providerA->SetAvailable(true);
    g_providerA->SetValidityDuration(Seconds(4.0));

    g_providerB = CreateObject<SixGCapabilityProvider>();
    g_providerB->SetCore(g_core);
    g_providerB->SetNodeId("EdgeNode-2");
    g_providerB->SetCapabilityType("COMPUTING");
    g_edgeNode2->SetComputeCapacity(80.0); g_edgeNode2->SetAvailableCompute(80.0); g_providerB->SetResource(g_edgeNode2->GetResource("COMPUTING"));
    g_providerB->SetAvailable(true);
    g_providerB->SetValidityDuration(Seconds(10.0));

    bool resultA = g_providerA->Register();
    bool resultB = g_providerB->Register();

    std::cout << "    EdgeNode-1: "
              << (resultA ? "SUCCESS" : "FAILED")
              << " | capacity="
              << g_providerA->GetCapability().capacity
              << " | available="
              << g_providerA->GetCapability().availableResource
              << std::endl;

    std::cout << "    EdgeNode-2: "
              << (resultB ? "SUCCESS" : "FAILED")
              << " | capacity="
              << g_providerB->GetCapability().capacity
              << " | available="
              << g_providerB->GetCapability().availableResource
              << std::endl;
}

void
DiscoverAndMatch(double minimumResource)
{
    SixGCapabilityRequest request;
    request.type = "COMPUTING";
    request.minimumResource = minimumResource;
    request.requireAvailable = true;
    request.requireFresh = false;

    std::cout << "[t=" << Simulator::Now().GetSeconds()
              << "s] Capability discovery | "
              << request.type
              << " >= "
              << request.minimumResource
              << std::endl;

    std::vector<std::string> candidateNodeIds; for (uint32_t i = 0; i < g_accessNode->GetReachableEdgeNodeCount(); ++i) { auto edgeNode = g_accessNode->GetReachableEdgeNode(i); if (edgeNode) { candidateNodeIds.push_back(edgeNode->GetNodeId()); } } auto discovered = g_core->DiscoverCapabilities(request, candidateNodeIds);

    std::cout << "    Discovered candidates: "
              << discovered.size()
              << std::endl;

    for (const auto& capability : discovered)
    {
        std::cout << "        "
                  << capability.nodeId
                  << " | capacity="
                  << capability.capacity
                  << " | available="
                  << capability.availableResource
                  << std::endl;
    }

    auto ranked = g_matcher->Match(request, discovered);

    std::cout << "    Ranked candidates:" << std::endl;

    for (std::size_t i = 0; i < ranked.size(); ++i)
    {
        std::cout << "        Rank "
                  << (i + 1)
                  << ": "
                  << ranked[i].nodeId
                  << " | available="
                  << ranked[i].availableResource
                  << std::endl;
    }

    if (!ranked.empty())
    {
        std::cout << "    Selected node: "
                  << ranked.front().nodeId
                  << std::endl;
    }
    else
    {
        std::cout << "    Selected node: NONE"
                  << std::endl;
    }

    std::cout << std::endl;
}

void
FreshnessExperiment()
{
    SixGCapabilityRequest request;
    request.type = "COMPUTING";
    request.minimumResource = 50.0;
    request.requireAvailable = true;

    std::cout << "[t=" << Simulator::Now().GetSeconds()
              << "s] Freshness experiment" << std::endl;

    request.requireFresh = false;

    auto allMatches = g_core->DiscoverCapabilities(request);

    std::cout << "    Normal discovery (requireFresh=false): "
              << allMatches.size()
              << " matches" << std::endl;

    for (const auto& capability : allMatches)
    {
        std::cout << "        "
                  << capability.nodeId
                  << " | lastUpdated="
                  << capability.lastUpdated.GetSeconds()
                  << "s | validity="
                  << capability.validityDuration.GetSeconds()
                  << "s | fresh="
                  << (g_core->IsCapabilityFresh(capability) ? "YES" : "NO")
                  << std::endl;
    }

    request.requireFresh = true;

    auto freshMatches = g_core->DiscoverCapabilities(request);

    std::cout << "    Fresh-only discovery (requireFresh=true): "
              << freshMatches.size()
              << " matches" << std::endl;

    for (const auto& capability : freshMatches)
    {
        std::cout << "        "
                  << capability.nodeId
                  << std::endl;
    }

    std::cout << std::endl;
}

void
StartSelectedTask()
{
    SixGCapabilityRequest request;
    request.type = "COMPUTING";
    request.minimumResource = g_service->GetRequiredCompute();
    request.requireAvailable = true;

    std::vector<std::string> candidateNodeIds;

    for (uint32_t i = 0; i < g_accessNode->GetReachableEdgeNodeCount(); ++i)
    {
        auto edgeNode = g_accessNode->GetReachableEdgeNode(i);

        if (edgeNode)
        {
            candidateNodeIds.push_back(edgeNode->GetNodeId());
        }
    }

    auto discovered =
        g_core->DiscoverCapabilities(request, candidateNodeIds);

    auto ranked = g_matcher->Match(request, discovered);

    std::cout << "[t=" << Simulator::Now().GetSeconds()
              << "s] Service capability selection"
              << std::endl;

    std::cout << "    Service: "
              << g_service->GetServiceId()
              << " | compute="
              << g_service->GetRequiredCompute()
              << " | communication="
              << g_service->GetRequiredCommunication()
              << std::endl;

    if (ranked.empty())
    {
        std::cout << "    Selected Edge Node: NONE"
                  << std::endl
                  << "    Service session ACTIVATION: FAILED"
                  << std::endl
                  << std::endl;
        return;
    }

    const std::string selectedNode = ranked.front().nodeId;

    Ptr<SixGEdgeNode> selectedEdgeNode;

    if (selectedNode == g_edgeNode1->GetNodeId())
    {
        selectedEdgeNode = g_edgeNode1;
    }
    else if (selectedNode == g_edgeNode2->GetNodeId())
    {
        selectedEdgeNode = g_edgeNode2;
    }

    if (!selectedEdgeNode)
    {
        std::cout << "    ERROR: Selected Edge Node not found"
                  << std::endl
                  << std::endl;
        return;
    }

    g_session->SetEdgeNode(selectedEdgeNode);
    if (selectedNode == g_edgeNode1->GetNodeId())
    {
        g_session->SetCapabilityProvider(g_providerA);
    }
    else if (selectedNode == g_edgeNode2->GetNodeId())
    {
        g_session->SetCapabilityProvider(g_providerB);
    }

    bool activated = g_session->Activate();

    std::cout << "    Selected Edge Node: "
              << selectedNode
              << std::endl;

    std::cout << "    Service session ACTIVATION: "
              << (activated ? "SUCCESS" : "FAILED")
              << std::endl;

    std::cout << "    Edge compute available: "
              << selectedEdgeNode->GetAvailableCompute()
              << std::endl;

    std::cout << "    Access communication available: "
              << g_accessNode->GetAvailableCommunication()
              << std::endl;

    std::cout << std::endl;
}

void
StartSecondService()
{
    SixGCapabilityRequest request;
    request.type = "COMPUTING";
    request.minimumResource = g_service2->GetRequiredCompute();
    request.requireAvailable = true;

    std::vector<std::string> candidateNodeIds;

    for (uint32_t i = 0; i < g_accessNode->GetReachableEdgeNodeCount(); ++i)
    {
        auto edgeNode = g_accessNode->GetReachableEdgeNode(i);

        if (edgeNode)
        {
            candidateNodeIds.push_back(edgeNode->GetNodeId());
        }
    }

    auto discovered =
        g_core->DiscoverCapabilities(request, candidateNodeIds);

    auto ranked = g_matcher->Match(request, discovered);

    std::cout << "[t=" << Simulator::Now().GetSeconds()
              << "s] Second service capability selection"
              << std::endl;

    std::cout << "    Service: "
              << g_service2->GetServiceId()
              << " | compute="
              << g_service2->GetRequiredCompute()
              << " | communication="
              << g_service2->GetRequiredCommunication()
              << std::endl;

    if (ranked.empty())
    {
        std::cout << "    Selected Edge Node: NONE"
                  << std::endl
                  << "    Service session ACTIVATION: FAILED"
                  << std::endl
                  << std::endl;
        return;
    }

    const std::string selectedNode = ranked.front().nodeId;

    Ptr<SixGEdgeNode> selectedEdgeNode;

    if (selectedNode == g_edgeNode1->GetNodeId())
    {
        selectedEdgeNode = g_edgeNode1;
        g_session2->SetCapabilityProvider(g_providerA);
    }
    else if (selectedNode == g_edgeNode2->GetNodeId())
    {
        selectedEdgeNode = g_edgeNode2;
        g_session2->SetCapabilityProvider(g_providerB);
    }

    if (!selectedEdgeNode)
    {
        std::cout << "    ERROR: Selected Edge Node not found"
                  << std::endl
                  << std::endl;
        return;
    }

    g_session2->SetEdgeNode(selectedEdgeNode);

    bool activated = g_session2->Activate();

    std::cout << "    Selected Edge Node: "
              << selectedNode
              << std::endl;

    std::cout << "    Service session ACTIVATION: "
              << (activated ? "SUCCESS" : "FAILED")
              << std::endl;

    std::cout << "    Edge compute available: "
              << selectedEdgeNode->GetAvailableCompute()
              << std::endl;

    std::cout << "    Access communication available: "
              << g_accessNode->GetAvailableCommunication()
              << std::endl;

    std::cout << std::endl;
}

void
FinishSecondService()
{
    bool deactivated = false;

    if (g_session2)
    {
        deactivated = g_session2->Deactivate();
    }

    std::cout << "[t=" << Simulator::Now().GetSeconds()
              << "s] Second service session DEACTIVATION: "
              << (deactivated ? "SUCCESS" : "FAILED")
              << std::endl;

    std::cout << "    EdgeNode-2 compute available: "
              << g_edgeNode2->GetAvailableCompute()
              << std::endl;

    std::cout << std::endl;
}

void
FinishSelectedTask()
{
    bool deactivated = false;

    if (g_session)
    {
        deactivated = g_session->Deactivate();
    }

    std::cout << "[t=" << Simulator::Now().GetSeconds()
              << "s] Service session DEACTIVATION: "
              << (deactivated ? "SUCCESS" : "FAILED")
              << std::endl;

    std::cout << "    EdgeNode-1 compute available: "
              << g_edgeNode1->GetAvailableCompute()
              << std::endl;

    std::cout << "    Access communication available: "
              << g_accessNode->GetAvailableCommunication()
              << std::endl;

    std::cout << std::endl;
}

int
main(int argc, char* argv[])
{
    CommandLine cmd(__FILE__);
    cmd.Parse(argc, argv);

    std::cout << "=== 6G-LENA Capability Lifecycle Demo ==="
              << std::endl;

    g_core = CreateObject<SixGCore>();
    g_core->SetScenario(
        "6G-Closed-Loop-Capability-Discovery-Matching");

    g_core->Initialize();

    g_matcher = CreateObject<SixGCapabilityMatcher>();

    std::cout << "Scenario: "
              << g_core->GetScenario()
              << std::endl;

    std::cout << "Initialized: "
              << g_core->IsInitialized()
              << std::endl
              << std::endl;

    Simulator::Schedule(Seconds(0.0), &BuildNetworkTopology);
    Simulator::Schedule(Seconds(0.1), &RegisterNodes);
    Simulator::Schedule(Seconds(1.0),
                        &DiscoverAndMatch,
                        50.0);
    Simulator::Schedule(Seconds(2.0),
                        &StartSelectedTask);
    Simulator::Schedule(Seconds(3.0),
                        &StartSecondService);
    Simulator::Schedule(Seconds(4.0),
                        &FinishSelectedTask);
    Simulator::Schedule(Seconds(5.0),
                        &DiscoverAndMatch,
                        50.0);
    Simulator::Schedule(Seconds(5.5),
                        &FinishSecondService);

    Simulator::Schedule(Seconds(6.0), &FreshnessExperiment);
    Simulator::Schedule(Seconds(9.0), &FreshnessExperiment);
    Simulator::Schedule(Seconds(11.0), &FreshnessExperiment);

    Simulator::Stop(Seconds(12.0));
    Simulator::Run();
    Simulator::Destroy();

    std::cout << "=== Simulation completed ==="
              << std::endl;

    return 0;
}
