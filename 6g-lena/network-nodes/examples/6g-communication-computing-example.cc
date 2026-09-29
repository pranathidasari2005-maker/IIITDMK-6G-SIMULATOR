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
    std::cout << "   6G-LENA COMMUNICATION + COMPUTING\n";
    std::cout << "============================================\n";

    Ptr<SixGAccessNode> accessNode =
        CreateObject<SixGAccessNode>();
    accessNode->SetNodeId("AccessNode-1");

    Ptr<SixGEdgeNode> edgeNode =
        CreateObject<SixGEdgeNode>();
    edgeNode->SetNodeId("EdgeNode-1");

    Ptr<SixGUe> ue =
        CreateObject<SixGUe>();
    ue->SetNodeId("UE-1");

    Ptr<SixGService> service =
        CreateObject<SixGService>();
    service->SetServiceId("AI-Service-1");
    service->SetRequiredCommunication(30.0);
    service->SetRequiredCompute(50.0);
    service->SetActive(true);

    Ptr<SixGResource> communicationResource =
        accessNode->GetResource("COMMUNICATION");

    Ptr<SixGResource> computingResource =
        edgeNode->GetResource("COMPUTING");

    if (communicationResource == nullptr ||
        computingResource == nullptr)
    {
        std::cout << "Required resources not available\n";
        return 1;
    }

    communicationResource->SetCapacity(100.0);
    communicationResource->SetAvailable(100.0);

    computingResource->SetCapacity(100.0);
    computingResource->SetAvailable(100.0);

    accessNode->AddReachableEdgeNode(edgeNode);
    ue->SetConnectedAccessNode(accessNode);

    std::cout << "UE                  : "
              << ue->GetNodeId() << "\n";

    std::cout << "Access Node         : "
              << accessNode->GetNodeId() << "\n";

    std::cout << "Edge Node           : "
              << edgeNode->GetNodeId() << "\n";

    std::cout << "Service             : "
              << service->GetServiceId() << "\n";

    std::cout << "Required communication : "
              << service->GetRequiredCommunication() << "\n";

    std::cout << "Required computing     : "
              << service->GetRequiredCompute() << "\n";

    std::cout << "\nBefore service activation:\n";

    Ptr<SixGResource> communication =
        accessNode->GetResource("COMMUNICATION");

    Ptr<SixGResource> computing =
        edgeNode->GetResource("COMPUTING");

    std::cout << "Communication available : "
              << communication->GetAvailable() << "\n";

    std::cout << "Computing available     : "
              << computing->GetAvailable() << "\n";

    bool communicationReserved =
        communication->Consume(service->GetRequiredCommunication());

    bool computingReserved =
        computing->Consume(service->GetRequiredCompute());

    bool activation =
        communicationReserved && computingReserved;

    if (!activation)
    {
        if (communicationReserved)
        {
            communication->Release(
                service->GetRequiredCommunication());
        }

        if (computingReserved)
        {
            computing->Release(
                service->GetRequiredCompute());
        }
    }

    std::cout << "\nService activation     : "
              << (activation ? "SUCCESS" : "FAILED") << "\n";

    std::cout << "Communication remaining : "
              << communication->GetAvailable() << "\n";

    std::cout << "Computing remaining     : "
              << computing->GetAvailable() << "\n";

    std::cout << "\n============================================\n";
    std::cout << " Communication + Computing Example Complete\n";
    std::cout << "============================================\n";

    return 0;
}
