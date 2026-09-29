#include "ns3/core-module.h"
#include "ns3/network-nodes-module.h"
#include "ns3/6g-core.h"

#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

using namespace ns3;

struct PlacementResult
{
    uint32_t successful = 0;
    uint32_t failed = 0;
    double requestedCompute = 0.0;
    double allocatedCompute = 0.0;
};

static PlacementResult
RunDynamicPlacementExperiment(uint32_t repetitions)
{
    PlacementResult result;

    Ptr<SixGCore> core = CreateObject<SixGCore>();

    Ptr<SixGAccessNode> accessNode =
        CreateObject<SixGAccessNode>();
    accessNode->SetNodeId("AccessNode-1");

    Ptr<SixGUe> ue =
        CreateObject<SixGUe>();
    ue->SetNodeId("UE-1");

    Ptr<SixGEdgeNode> edgeNode1 =
        CreateObject<SixGEdgeNode>();
    edgeNode1->SetNodeId("EdgeNode-1");

    Ptr<SixGEdgeNode> edgeNode2 =
        CreateObject<SixGEdgeNode>();
    edgeNode2->SetNodeId("EdgeNode-2");

    Ptr<SixGEdgeNode> edgeNode3 =
        CreateObject<SixGEdgeNode>();
    edgeNode3->SetNodeId("EdgeNode-3");

    edgeNode1->GetResource("COMPUTING")->SetCapacity(80.0);
    edgeNode1->GetResource("COMPUTING")->SetAvailable(80.0);

    edgeNode2->GetResource("COMPUTING")->SetCapacity(120.0);
    edgeNode2->GetResource("COMPUTING")->SetAvailable(120.0);

    edgeNode3->GetResource("COMPUTING")->SetCapacity(50.0);
    edgeNode3->GetResource("COMPUTING")->SetAvailable(50.0);

    accessNode->GetResource("COMMUNICATION")->SetCapacity(100.0);
    accessNode->GetResource("COMMUNICATION")->SetAvailable(100.0);

    accessNode->AddReachableEdgeNode(edgeNode1);
    accessNode->AddReachableEdgeNode(edgeNode2);
    accessNode->AddReachableEdgeNode(edgeNode3);

    ue->SetConnectedAccessNode(accessNode);

    Ptr<SixGService> service =
        CreateObject<SixGService>();

    service->SetServiceId("Service-1");
    service->SetRequiredCompute(70.0);
    service->SetRequiredCommunication(20.0);
    service->SetActive(true);

    std::vector<Ptr<SixGEdgeNode>> edgeNodes =
        {edgeNode1, edgeNode2, edgeNode3};

    /*
     * Register the initial capabilities.
     */
    for (const auto& edge : edgeNodes)
    {
        SixGCapability capability;
        capability.nodeId = edge->GetNodeId();
        capability.type = "COMPUTING";
        capability.capacity =
            edge->GetResource("COMPUTING")->GetCapacity();
        capability.availableResource =
            edge->GetResource("COMPUTING")->GetAvailable();
        capability.available =
            capability.availableResource >= 70.0;
        capability.state =
            capability.available
                ? SixGCapabilityState::ACTIVE
                : SixGCapabilityState::DEGRADED;
        capability.lastUpdated = Simulator::Now();
        capability.validityDuration = Seconds(0);

        core->RegisterCapability(capability);
    }

    for (uint32_t request = 1; request <= repetitions; ++request)
    {
        result.requestedCompute +=
            service->GetRequiredCompute();

        SixGCapabilityRequest capabilityRequest;
        capabilityRequest.type = "COMPUTING";
        capabilityRequest.minimumResource =
            service->GetRequiredCompute();
        capabilityRequest.requireAvailable = true;

        std::vector<SixGCapability> candidates =
            core->DiscoverCapabilities(capabilityRequest);

        Ptr<SixGEdgeNode> selectedEdge = nullptr;
        double bestAvailable = -1.0;

        for (const auto& capability : candidates)
        {
            for (const auto& edge : edgeNodes)
            {
                if (edge->GetNodeId() == capability.nodeId)
                {
                    double available =
                        edge->GetResource("COMPUTING")->GetAvailable();

                    if (available > bestAvailable)
                    {
                        bestAvailable = available;
                        selectedEdge = edge;
                    }
                }
            }
        }

        if (selectedEdge == nullptr)
        {
            result.failed++;

            std::cout
                << "request=" << request
                << ",selected_node=NONE"
                << ",status=FAILED"
                << std::endl;

            continue;
        }

        Ptr<SixGServiceSession> session =
            CreateObject<SixGServiceSession>();

        session->SetUe(ue);
        session->SetAccessNode(accessNode);
        session->SetEdgeNode(selectedEdge);
        session->SetService(service);

        bool activated = session->Activate();

        if (!activated)
        {
            result.failed++;

            std::cout
                << "request=" << request
                << ",selected_node="
                << selectedEdge->GetNodeId()
                << ",status=FAILED"
                << std::endl;

            continue;
        }

        result.successful++;
        result.allocatedCompute +=
            service->GetRequiredCompute();

        /*
         * Synchronize the selected provider's capability
         * with its actual remaining resource state.
         */
        SixGCapability updatedCapability;

        updatedCapability.nodeId =
            selectedEdge->GetNodeId();

        updatedCapability.type = "COMPUTING";

        updatedCapability.capacity =
            selectedEdge->GetResource("COMPUTING")->GetCapacity();

        updatedCapability.availableResource =
            selectedEdge->GetResource("COMPUTING")->GetAvailable();

        updatedCapability.available =
            updatedCapability.availableResource >=
            service->GetRequiredCompute();

        updatedCapability.state =
            updatedCapability.available
                ? SixGCapabilityState::ACTIVE
                : SixGCapabilityState::DEGRADED;

        updatedCapability.lastUpdated =
            Simulator::Now();

        updatedCapability.validityDuration =
            Seconds(0);

        core->UpdateCapability(updatedCapability);

        /*
         * Deactivate the session so the experiment models
         * persistent resource consumption while avoiding
         * a permanently active session object.
         *
         * The compute resource is intentionally restored by
         * the session, so we consume the service allocation
         * explicitly below to represent persistent placement.
         */
        session->Deactivate();

        selectedEdge->GetResource("COMPUTING")
            ->Consume(service->GetRequiredCompute());

        updatedCapability.availableResource =
            selectedEdge->GetResource("COMPUTING")->GetAvailable();

        updatedCapability.available =
            updatedCapability.availableResource >=
            service->GetRequiredCompute();

        updatedCapability.state =
            updatedCapability.available
                ? SixGCapabilityState::ACTIVE
                : SixGCapabilityState::DEGRADED;

        updatedCapability.lastUpdated =
            Simulator::Now();

        core->UpdateCapability(updatedCapability);

        std::cout
            << "request=" << request
            << ",selected_node="
            << selectedEdge->GetNodeId()
            << ",status=SUCCESS"
            << ",remaining_compute="
            << std::fixed << std::setprecision(2)
            << updatedCapability.availableResource
            << std::endl;
    }

    return result;
}

int
main(int argc, char* argv[])
{
    uint32_t repetitions = 5;

    CommandLine cmd(__FILE__);
    cmd.AddValue(
        "repetitions",
        "Number of service placement requests",
        repetitions);
    cmd.Parse(argc, argv);

    std::cout
        << "6G-LENA Dynamic Capability-Based Service Placement"
        << std::endl;

    std::cout
        << "research_question=Can capability discovery select "
           "currently suitable edge providers for service placement?"
        << std::endl;

    std::cout
        << "provider_capacities=80,120,50"
        << std::endl;

    std::cout
        << "required_compute=70"
        << std::endl;

    std::cout
        << "repetitions=" << repetitions
        << std::endl;

    std::cout << std::endl;

    PlacementResult result =
        RunDynamicPlacementExperiment(repetitions);

    double successRate = 0.0;

    if (repetitions > 0)
    {
        successRate =
            100.0 *
            static_cast<double>(result.successful) /
            static_cast<double>(repetitions);
    }

    double allocationRatio = 0.0;

    if (result.requestedCompute > 0.0)
    {
        allocationRatio =
            100.0 *
            result.allocatedCompute /
            result.requestedCompute;
    }

    std::cout << std::endl;

    std::cout
        << "SUMMARY"
        << std::endl;

    std::cout
        << "requests=" << repetitions
        << std::endl;

    std::cout
        << "successful=" << result.successful
        << std::endl;

    std::cout
        << "failed=" << result.failed
        << std::endl;

    std::cout
        << "success_rate="
        << std::fixed << std::setprecision(2)
        << successRate
        << "%"
        << std::endl;

    std::cout
        << "requested_compute="
        << result.requestedCompute
        << std::endl;

    std::cout
        << "allocated_compute="
        << result.allocatedCompute
        << std::endl;

    std::cout
        << "allocation_ratio="
        << allocationRatio
        << "%"
        << std::endl;

    return 0;
}
