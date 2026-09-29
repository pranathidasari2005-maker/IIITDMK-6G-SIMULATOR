#include "ns3/core-module.h"
#include "ns3/network-nodes-module.h"

#include <iostream>

using namespace ns3;

int
main(int argc, char* argv[])
{
    CommandLine cmd(__FILE__);
    cmd.Parse(argc, argv);

    std::cout << "\n============================================\n";
    std::cout << "       6G-LENA SERVICE PLACEMENT\n";
    std::cout << "============================================\n";

    Ptr<SixGAccessNode> accessNode =
        CreateObject<SixGAccessNode>();
    accessNode->SetNodeId("AccessNode-1");

    Ptr<SixGUe> ue =
        CreateObject<SixGUe>();
    ue->SetNodeId("UE-1");
    ue->SetConnectedAccessNode(accessNode);

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

    Ptr<SixGService> service =
        CreateObject<SixGService>();

    service->SetServiceId("AI-Service-1");
    service->SetRequiredCompute(70.0);
    service->SetRequiredCommunication(20.0);
    service->SetActive(true);

    std::cout << "\nService request:\n";
    std::cout << "  Service          : "
              << service->GetServiceId() << "\n";
    std::cout << "  Required compute : "
              << service->GetRequiredCompute() << "\n";
    std::cout << "  Required communication : "
              << service->GetRequiredCommunication() << "\n";

    Ptr<SixGEdgeNode> selectedEdge = nullptr;
    double bestAvailableCompute = -1.0;

    for (uint32_t i = 0;
         i < accessNode->GetReachableEdgeNodeCount();
         ++i)
    {
        Ptr<SixGEdgeNode> candidate =
            accessNode->GetReachableEdgeNode(i);

        double available =
            candidate->GetResource("COMPUTING")->GetAvailable();

        std::cout << "\nCandidate: "
                  << candidate->GetNodeId()
                  << " | available compute = "
                  << available;

        if (available >= service->GetRequiredCompute() &&
            available > bestAvailableCompute)
        {
            selectedEdge = candidate;
            bestAvailableCompute = available;
        }
    }

    std::cout << "\n";

    if (selectedEdge == nullptr)
    {
        std::cout << "\nService placement: FAILED\n";
        return 1;
    }

    std::cout << "\nSelected Edge Node : "
              << selectedEdge->GetNodeId() << "\n";

    Ptr<SixGServiceSession> session =
        CreateObject<SixGServiceSession>();

    session->SetUe(ue);
    session->SetAccessNode(accessNode);
    session->SetEdgeNode(selectedEdge);
    session->SetService(service);

    bool activated = session->Activate();

    std::cout << "\nService activation: "
              << (activated ? "SUCCESS" : "FAILED") << "\n";

    if (!activated)
    {
        return 1;
    }

    std::cout << "\nPlacement result:\n";
    std::cout << "  UE             : "
              << ue->GetNodeId() << "\n";
    std::cout << "  Access Node    : "
              << accessNode->GetNodeId() << "\n";
    std::cout << "  Edge Node      : "
              << selectedEdge->GetNodeId() << "\n";
    std::cout << "  Service active : "
              << (service->IsActive() ? "YES" : "NO") << "\n";
    std::cout << "  Compute left   : "
              << selectedEdge->GetResource("COMPUTING")->GetAvailable()
              << "\n";
    std::cout << "  Communication left : "
              << accessNode->GetResource("COMMUNICATION")->GetAvailable()
              << "\n";

    std::cout << "\n============================================\n";
    std::cout << "       SERVICE PLACEMENT COMPLETE\n";
    std::cout << "============================================\n";

    return 0;
}
