#include "ns3/6g-access-node.h"
#include "ns3/6g-edge-node.h"
#include "ns3/6g-service-session.h"
#include "ns3/6g-service.h"
#include "ns3/6g-ue.h"
#include "ns3/core-module.h"

#include <iostream>

using namespace ns3;

int
main(int argc, char* argv[])
{
    CommandLine cmd(__FILE__);
    cmd.Parse(argc, argv);

    Ptr<SixGAccessNode> accessNode = CreateObject<SixGAccessNode>();
    Ptr<SixGEdgeNode> reachableEdge = CreateObject<SixGEdgeNode>();
    Ptr<SixGEdgeNode> unreachableEdge = CreateObject<SixGEdgeNode>();
    Ptr<SixGUe> ue = CreateObject<SixGUe>();
    Ptr<SixGService> service = CreateObject<SixGService>();

    accessNode->SetNodeId("AccessNode-1");
    reachableEdge->SetNodeId("EdgeNode-1");
    unreachableEdge->SetNodeId("EdgeNode-2");
    ue->SetNodeId("UE-1");

    /*
     * Activate all participating entities.
     */
    accessNode->SetActive(true);
    reachableEdge->SetActive(true);
    ue->SetActive(true);

    service->SetServiceId("Service-1");
    service->SetRequiredCommunication(20.0);
    service->SetRequiredCompute(30.0);
    service->SetActive(true);

    /*
     * Build the actual network relationships.
     */
    accessNode->AddReachableEdgeNode(reachableEdge);
    ue->SetConnectedAccessNode(accessNode);

    /*
     * Give the Access and Edge nodes sufficient resources.
     */
    accessNode->SetCommunicationCapacity(100.0);
    accessNode->SetAvailableCommunication(100.0);

    reachableEdge->SetComputeCapacity(100.0);
    reachableEdge->SetAvailableCompute(100.0);

    unreachableEdge->SetComputeCapacity(100.0);
    unreachableEdge->SetAvailableCompute(100.0);

    /*
     * Create a service session using the reachable path.
     */
    Ptr<SixGServiceSession> validSession =
        CreateObject<SixGServiceSession>();

    validSession->SetSessionId("Session-1");
    validSession->SetUe(ue);
    validSession->SetAccessNode(accessNode);
    validSession->SetEdgeNode(reachableEdge);
    validSession->SetService(service);

    bool validActivation = validSession->Activate();

    std::cout << "============================================" << std::endl;
    std::cout << "       6G-LENA SERVICE PATH TEST" << std::endl;
    std::cout << "============================================" << std::endl;

    std::cout << "UE                   : "
              << ue->GetNodeId() << std::endl;

    std::cout << "Serving Access Node  : "
              << ue->GetConnectedAccessNode() << std::endl;

    std::cout << "Reachable Edge Node  : "
              << reachableEdge->GetNodeId() << std::endl;

    std::cout << "Service               : "
              << service->GetServiceId() << std::endl;

    std::cout << "Valid path activation : "
              << (validActivation ? "SUCCESS" : "FAILED") << std::endl;

    std::cout << "Session active        : "
              << (validSession->IsActive() ? "YES" : "NO") << std::endl;

    std::cout << "Communication left    : "
              << accessNode->GetAvailableCommunication() << std::endl;

    std::cout << "Compute left          : "
              << reachableEdge->GetAvailableCompute() << std::endl;

    /*
     * Deactivate before testing the invalid path.
     */
    validSession->Deactivate();

    /*
     * Try to establish the same service through an Edge Node
     * that is NOT reachable from the Access Node.
     */
    Ptr<SixGServiceSession> invalidSession =
        CreateObject<SixGServiceSession>();

    invalidSession->SetSessionId("Session-2");
    invalidSession->SetUe(ue);
    invalidSession->SetAccessNode(accessNode);
    invalidSession->SetEdgeNode(unreachableEdge);
    invalidSession->SetService(service);

    bool invalidActivation = invalidSession->Activate();

    std::cout << "--------------------------------------------" << std::endl;

    std::cout << "Unreachable Edge     : "
              << unreachableEdge->GetNodeId() << std::endl;

    std::cout << "Invalid path rejected: "
              << (!invalidActivation ? "YES" : "NO") << std::endl;

    std::cout << "============================================" << std::endl;

    return 0;
}
