#include "ns3/core-module.h"
#include "ns3/network-nodes-module.h"

#include <iostream>

using namespace ns3;

int
main(int argc, char* argv[])
{
    double communicationCapacity = 100.0;
    double computingCapacity = 100.0;
    double communicationRequired = 30.0;
    double computingRequired = 40.0;

    CommandLine cmd;
    cmd.AddValue(
        "communicationCapacity",
        "Communication resource capacity",
        communicationCapacity);
    cmd.AddValue(
        "computingCapacity",
        "Computing resource capacity",
        computingCapacity);
    cmd.AddValue(
        "communicationRequired",
        "Communication resource required by the service",
        communicationRequired);
    cmd.AddValue(
        "computingRequired",
        "Computing resource required by the service",
        computingRequired);
    cmd.Parse(argc, argv);

    std::cout << "\n============================================\n";
    std::cout << "  6G-LENA END-TO-END SERVICE SESSION EXPERIMENT\n";
    std::cout << "============================================\n";

    // ------------------------------------------------------------
    // Network topology
    // ------------------------------------------------------------
    Ptr<SixGAccessNode> accessNode = CreateObject<SixGAccessNode>();
    Ptr<SixGEdgeNode> edgeNode = CreateObject<SixGEdgeNode>();
    Ptr<SixGUe> ue = CreateObject<SixGUe>();
    Ptr<SixGService> service = CreateObject<SixGService>();

    accessNode->AddReachableEdgeNode(edgeNode);
    ue->SetConnectedAccessNode(accessNode);

    // ------------------------------------------------------------
    // Resource configuration
    // ------------------------------------------------------------
    auto communication = accessNode->GetResource("COMMUNICATION");
    auto computing = edgeNode->GetResource("COMPUTING");

    communication->SetCapacity(communicationCapacity);
    communication->SetAvailable(communicationCapacity);

    computing->SetCapacity(computingCapacity);
    computing->SetAvailable(computingCapacity);

    service->SetRequiredCommunication(communicationRequired);
    service->SetRequiredCompute(computingRequired);
    service->SetActive(true);

    std::cout << "\nInitial resource state:\n";
    std::cout << "  Communication capacity : "
              << communication->GetCapacity() << "\n";
    std::cout << "  Communication available: "
              << communication->GetAvailable() << "\n";
    std::cout << "  Computing capacity     : "
              << computing->GetCapacity() << "\n";
    std::cout << "  Computing available    : "
              << computing->GetAvailable() << "\n";

    // ------------------------------------------------------------
    // Service session
    // ------------------------------------------------------------
    Ptr<SixGServiceSession> session = CreateObject<SixGServiceSession>();

    session->SetUe(ue);
    session->SetAccessNode(accessNode);
    session->SetEdgeNode(edgeNode);
    session->SetService(service);

    std::cout << "\nService requirements:\n";
    std::cout << "  Communication required: "
              << service->GetRequiredCommunication() << "\n";
    std::cout << "  Computing required    : "
              << service->GetRequiredCompute() << "\n";

    const double communicationBefore = communication->GetAvailable();
    const double computingBefore = computing->GetAvailable();

    // ------------------------------------------------------------
    // Activate service
    // ------------------------------------------------------------
    const bool activated = session->Activate();

    std::cout << "\nService activation: "
              << (activated ? "SUCCESS" : "FAILED") << "\n";

    const double communicationAfterActivation =
        communication->GetAvailable();

    const double computingAfterActivation =
        computing->GetAvailable();

    std::cout << "  Communication available after activation: "
              << communicationAfterActivation << "\n";
    std::cout << "  Computing available after activation    : "
              << computingAfterActivation << "\n";

    // ------------------------------------------------------------
    // Deactivate service
    // ------------------------------------------------------------
    bool deactivated = false;

    if (activated)
    {
        deactivated = session->Deactivate();
    }

    std::cout << "\nService deactivation: ";

    if (activated)
    {
        std::cout << (deactivated ? "SUCCESS" : "FAILED") << "\n";
    }
    else
    {
        std::cout << "NOT_REQUIRED (activation rejected)\n";
    }

    const double communicationAfterRelease =
        communication->GetAvailable();

    const double computingAfterRelease =
        computing->GetAvailable();

    std::cout << "  Communication available after release: "
              << communicationAfterRelease << "\n";
    std::cout << "  Computing available after release    : "
              << computingAfterRelease << "\n";

    // ------------------------------------------------------------
    // Validation
    // ------------------------------------------------------------
    const bool activationSucceededCorrectly =
        activated &&
        communicationAfterActivation ==
            communicationBefore - service->GetRequiredCommunication() &&
        computingAfterActivation ==
            computingBefore - service->GetRequiredCompute();

    const bool activationRejectedSafely =
        !activated &&
        communicationAfterActivation == communicationBefore &&
        computingAfterActivation == computingBefore;

    const bool releaseSucceededCorrectly =
        activated &&
        deactivated &&
        communicationAfterRelease == communicationBefore &&
        computingAfterRelease == computingBefore;

    const bool releaseNotRequired =
        !activated &&
        communicationAfterRelease == communicationBefore &&
        computingAfterRelease == computingBefore;

    const bool activationResourceCheck =
        activationSucceededCorrectly || activationRejectedSafely;

    const bool releaseResourceCheck =
        releaseSucceededCorrectly || releaseNotRequired;

    const bool passed =
        activationResourceCheck && releaseResourceCheck;

    std::cout << "\nExperiment result:\n";
    std::cout << "  Activation resource check : "
              << (activationResourceCheck ? "PASS" : "FAIL") << "\n";
    std::cout << "  Release resource check    : "
              << (releaseResourceCheck ? "PASS" : "FAIL") << "\n";

    std::cout << "\n============================================\n";

    if (passed)
    {
        std::cout << " END-TO-END SESSION EXPERIMENT PASSED\n";
    }
    else
    {
        std::cout << " END-TO-END SESSION EXPERIMENT FAILED\n";
    }

    std::cout << "============================================\n";

    return passed ? 0 : 1;
}
