#include "ns3/core-module.h"
#include "ns3/network-nodes-module.h"

#include <iostream>
#include <iomanip>

using namespace ns3;

int
main(int argc, char* argv[])
{
    double access1CommunicationCapacity = 100.0;
    double access2CommunicationCapacity = 60.0;

    double edge1ComputingCapacity = 120.0;
    double edge2ComputingCapacity = 80.0;
    double edge3ComputingCapacity = 40.0;

    CommandLine cmd;
    cmd.AddValue("access1CommunicationCapacity",
                 "AccessNode-1 communication capacity",
                 access1CommunicationCapacity);
    cmd.AddValue("access2CommunicationCapacity",
                 "AccessNode-2 communication capacity",
                 access2CommunicationCapacity);
    cmd.AddValue("edge1ComputingCapacity",
                 "EdgeNode-1 computing capacity",
                 edge1ComputingCapacity);
    cmd.AddValue("edge2ComputingCapacity",
                 "EdgeNode-2 computing capacity",
                 edge2ComputingCapacity);
    cmd.AddValue("edge3ComputingCapacity",
                 "EdgeNode-3 computing capacity",
                 edge3ComputingCapacity);
    cmd.Parse(argc, argv);

    if (access1CommunicationCapacity <= 0.0 ||
        access2CommunicationCapacity <= 0.0 ||
        edge1ComputingCapacity <= 0.0 ||
        edge2ComputingCapacity <= 0.0 ||
        edge3ComputingCapacity <= 0.0)
    {
        std::cerr << "All resource capacities must be greater than zero.\n";
        return 1;
    }

    std::cout << "\n============================================\n";
    std::cout << "  6G-LENA HETEROGENEOUS NETWORK EXPERIMENT\n";
    std::cout << "============================================\n";

    Ptr<SixGAccessNode> access1 = CreateObject<SixGAccessNode>();
    Ptr<SixGAccessNode> access2 = CreateObject<SixGAccessNode>();

    Ptr<SixGEdgeNode> edge1 = CreateObject<SixGEdgeNode>();
    Ptr<SixGEdgeNode> edge2 = CreateObject<SixGEdgeNode>();
    Ptr<SixGEdgeNode> edge3 = CreateObject<SixGEdgeNode>();

    access1->AddReachableEdgeNode(edge1);
    access1->AddReachableEdgeNode(edge2);
    access2->AddReachableEdgeNode(edge2);
    access2->AddReachableEdgeNode(edge3);

    access1->GetResource("COMMUNICATION")->SetCapacity(
        access1CommunicationCapacity);
    access1->GetResource("COMMUNICATION")->SetAvailable(
        access1CommunicationCapacity);

    access2->GetResource("COMMUNICATION")->SetCapacity(
        access2CommunicationCapacity);
    access2->GetResource("COMMUNICATION")->SetAvailable(
        access2CommunicationCapacity);

    edge1->GetResource("COMPUTING")->SetCapacity(
        edge1ComputingCapacity);
    edge1->GetResource("COMPUTING")->SetAvailable(
        edge1ComputingCapacity);

    edge2->GetResource("COMPUTING")->SetCapacity(
        edge2ComputingCapacity);
    edge2->GetResource("COMPUTING")->SetAvailable(
        edge2ComputingCapacity);

    edge3->GetResource("COMPUTING")->SetCapacity(
        edge3ComputingCapacity);
    edge3->GetResource("COMPUTING")->SetAvailable(
        edge3ComputingCapacity);

    std::cout << std::fixed << std::setprecision(2);

    std::cout << "\nHeterogeneous topology:\n";
    std::cout << "  Access nodes : 2\n";
    std::cout << "  Edge nodes   : 3\n";
    std::cout << "  AccessNode-1 reachable edges: "
              << access1->GetReachableEdgeNodeCount() << "\n";
    std::cout << "  AccessNode-2 reachable edges: "
              << access2->GetReachableEdgeNodeCount() << "\n";

    std::cout << "\nCommunication capacities:\n";
    std::cout << "  AccessNode-1: "
              << access1->GetResource("COMMUNICATION")->GetCapacity() << "\n";
    std::cout << "  AccessNode-2: "
              << access2->GetResource("COMMUNICATION")->GetCapacity() << "\n";

    std::cout << "\nComputing capacities:\n";
    std::cout << "  EdgeNode-1: "
              << edge1->GetResource("COMPUTING")->GetCapacity() << "\n";
    std::cout << "  EdgeNode-2: "
              << edge2->GetResource("COMPUTING")->GetCapacity() << "\n";
    std::cout << "  EdgeNode-3: "
              << edge3->GetResource("COMPUTING")->GetCapacity() << "\n";

    const bool topologyValid =
        access1->GetReachableEdgeNodeCount() == 2 &&
        access2->GetReachableEdgeNodeCount() == 2 &&
        access1->HasReachableEdgeNode(edge1) &&
        access1->HasReachableEdgeNode(edge2) &&
        access2->HasReachableEdgeNode(edge2) &&
        access2->HasReachableEdgeNode(edge3);

    const bool communicationHeterogeneity =
        access1CommunicationCapacity != access2CommunicationCapacity;

    const bool computingHeterogeneity =
        edge1ComputingCapacity != edge2ComputingCapacity &&
        edge2ComputingCapacity != edge3ComputingCapacity;

    const bool resourceHeterogeneity =
        communicationHeterogeneity &&
        computingHeterogeneity;

    const bool passed =
        topologyValid &&
        resourceHeterogeneity;

    std::cout << "\nValidation:\n";
    std::cout << "  Topology connectivity check : "
              << (topologyValid ? "PASS" : "FAIL") << "\n";
    std::cout << "  Resource heterogeneity check : "
              << (resourceHeterogeneity ? "PASS" : "FAIL") << "\n";

    std::cout << "\n============================================\n";
    std::cout << (passed
                      ? " HETEROGENEOUS NETWORK EXPERIMENT PASSED\n"
                      : " HETEROGENEOUS NETWORK EXPERIMENT FAILED\n");
    std::cout << "============================================\n";

    return passed ? 0 : 1;
}
