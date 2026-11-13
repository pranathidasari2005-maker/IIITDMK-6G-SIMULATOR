#include "ns3/core-module.h"
#include "ns3/network-nodes-module.h"

#include <iostream>

using namespace ns3;

int
main(int argc, char* argv[])
{
    double access1CommunicationCapacity = 100.0;
    double access2CommunicationCapacity = 100.0;
    double serviceCommunicationRequirement = 20.0;
    double serviceComputeRequirement = 20.0;

    CommandLine cmd;

    cmd.AddValue(
        "access1CommunicationCapacity",
        "AccessNode-1 communication capacity",
        access1CommunicationCapacity);

    cmd.AddValue(
        "access2CommunicationCapacity",
        "AccessNode-2 communication capacity",
        access2CommunicationCapacity);

    cmd.AddValue(
        "serviceCommunicationRequirement",
        "Service communication requirement",
        serviceCommunicationRequirement);

    cmd.AddValue(
        "serviceComputeRequirement",
        "Service computing requirement",
        serviceComputeRequirement);

    cmd.Parse(argc, argv);

    std::cout << "\n============================================\n";
    std::cout << "  6G-LENA MOBILITY / HANDOVER EXPERIMENT\n";
    std::cout << "============================================\n";

    Ptr<SixGAccessNode> access1 = CreateObject<SixGAccessNode>();
    Ptr<SixGAccessNode> access2 = CreateObject<SixGAccessNode>();
    Ptr<SixGEdgeNode> edge1 = CreateObject<SixGEdgeNode>();
    Ptr<SixGEdgeNode> edge2 = CreateObject<SixGEdgeNode>();
    Ptr<SixGUe> ue = CreateObject<SixGUe>();
    Ptr<SixGService> service = CreateObject<SixGService>();

    access1->AddReachableEdgeNode(edge1);
    access2->AddReachableEdgeNode(edge2);

    access1->GetResource("COMMUNICATION")->SetCapacity(
        access1CommunicationCapacity);
    access1->GetResource("COMMUNICATION")->SetAvailable(
        access1CommunicationCapacity);

    access2->GetResource("COMMUNICATION")->SetCapacity(
        access2CommunicationCapacity);
    access2->GetResource("COMMUNICATION")->SetAvailable(
        access2CommunicationCapacity);

    edge1->GetResource("COMPUTING")->SetCapacity(100.0);
    edge1->GetResource("COMPUTING")->SetAvailable(100.0);

    edge2->GetResource("COMPUTING")->SetCapacity(100.0);
    edge2->GetResource("COMPUTING")->SetAvailable(100.0);

    service->SetActive(true);
    service->SetRequiredCommunication(
        serviceCommunicationRequirement);
    service->SetRequiredCompute(
        serviceComputeRequirement);

    ue->SetConnectedAccessNode(access1);

    Ptr<SixGServiceSession> session =
        CreateObject<SixGServiceSession>();

    session->SetUe(ue);
    session->SetAccessNode(access1);
    session->SetEdgeNode(edge1);
    session->SetService(service);

    const double oldAccessInitial =
        access1->GetResource("COMMUNICATION")->GetAvailable();

    const double newAccessInitial =
        access2->GetResource("COMMUNICATION")->GetAvailable();

    const bool initialActivation = session->Activate();

    std::cout << "\nInitial connection:\n";
    std::cout << "  AccessNode-1 -> EdgeNode-1\n";
    std::cout << "  Session activation: "
              << (initialActivation ? "SUCCESS" : "FAILED") << "\n";

    const bool oldSessionReleased =
        session->Deactivate();

    ue->SetConnectedAccessNode(access2);

    Ptr<SixGServiceSession> handoverSession =
        CreateObject<SixGServiceSession>();

    handoverSession->SetUe(ue);
    handoverSession->SetAccessNode(access2);
    handoverSession->SetEdgeNode(edge2);
    handoverSession->SetService(service);

    const bool handoverActivation =
        handoverSession->Activate();

    const bool connectedToNewAccess =
        ue->GetConnectedAccessNodeObject() == access2;

    const bool oldResourcesRestored =
        access1->GetResource("COMMUNICATION")->GetAvailable() ==
            oldAccessInitial;

    const bool newResourcesReserved =
        access2->GetResource("COMMUNICATION")->GetAvailable() ==
            newAccessInitial - service->GetRequiredCommunication();

    const bool passed =
        initialActivation &&
        oldSessionReleased &&
        handoverActivation &&
        connectedToNewAccess &&
        oldResourcesRestored &&
        newResourcesReserved;

    std::cout << "\nHandover:\n";
    std::cout << "  Old session release: "
              << (oldSessionReleased ? "PASS" : "FAIL") << "\n";
    std::cout << "  New AccessNode-2 connection: "
              << (connectedToNewAccess ? "PASS" : "FAIL") << "\n";
    std::cout << "  New session activation: "
              << (handoverActivation ? "PASS" : "FAIL") << "\n";
    std::cout << "  Old communication resource restored: "
              << (oldResourcesRestored ? "PASS" : "FAIL") << "\n";
    std::cout << "  New communication resource reserved: "
              << (newResourcesReserved ? "PASS" : "FAIL") << "\n";

    handoverSession->Deactivate();

    std::cout << "\n============================================\n";
    std::cout << (passed
                      ? " MOBILITY / HANDOVER EXPERIMENT PASSED\n"
                      : " MOBILITY / HANDOVER EXPERIMENT FAILED\n");
    std::cout << "============================================\n";

    return passed ? 0 : 1;
}
