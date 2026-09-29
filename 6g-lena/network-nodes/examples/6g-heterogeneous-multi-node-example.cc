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
    std::cout << "     6G-LENA HETEROGENEOUS MULTI-NODE\n";
    std::cout << "============================================\n";

    // -------------------------------------------------
    // Access Nodes
    // -------------------------------------------------

    Ptr<SixGAccessNode> accessNode1 =
        CreateObject<SixGAccessNode>();

    accessNode1->SetNodeId("AccessNode-1");

    Ptr<SixGAccessNode> accessNode2 =
        CreateObject<SixGAccessNode>();

    accessNode2->SetNodeId("AccessNode-2");

    // -------------------------------------------------
    // Edge Nodes
    // -------------------------------------------------

    Ptr<SixGEdgeNode> edgeNode1 =
        CreateObject<SixGEdgeNode>();

    edgeNode1->SetNodeId("EdgeNode-1");

    Ptr<SixGEdgeNode> edgeNode2 =
        CreateObject<SixGEdgeNode>();

    edgeNode2->SetNodeId("EdgeNode-2");

    Ptr<SixGEdgeNode> edgeNode3 =
        CreateObject<SixGEdgeNode>();

    edgeNode3->SetNodeId("EdgeNode-3");

    // -------------------------------------------------
    // Configure heterogeneous computing resources
    // -------------------------------------------------

    edgeNode1->GetResource("COMPUTING")
        ->SetCapacity(100.0);
    edgeNode1->GetResource("COMPUTING")
        ->SetAvailable(100.0);

    edgeNode2->GetResource("COMPUTING")
        ->SetCapacity(60.0);
    edgeNode2->GetResource("COMPUTING")
        ->SetAvailable(60.0);

    edgeNode3->GetResource("COMPUTING")
        ->SetCapacity(150.0);
    edgeNode3->GetResource("COMPUTING")
        ->SetAvailable(150.0);

    // -------------------------------------------------
    // Build heterogeneous access-to-edge topology
    // -------------------------------------------------

    accessNode1->AddReachableEdgeNode(edgeNode1);
    accessNode1->AddReachableEdgeNode(edgeNode2);

    accessNode2->AddReachableEdgeNode(edgeNode2);
    accessNode2->AddReachableEdgeNode(edgeNode3);

    // -------------------------------------------------
    // UEs
    // -------------------------------------------------

    Ptr<SixGUe> ue1 =
        CreateObject<SixGUe>();

    ue1->SetNodeId("UE-1");
    ue1->SetConnectedAccessNode(accessNode1);

    Ptr<SixGUe> ue2 =
        CreateObject<SixGUe>();

    ue2->SetNodeId("UE-2");
    ue2->SetConnectedAccessNode(accessNode2);

    // -------------------------------------------------
    // Display topology
    // -------------------------------------------------

    std::cout << "\nUE-1 topology:\n";
    std::cout << "  UE-1 -> "
              << ue1->GetConnectedAccessNodeObject()->GetNodeId()
              << "\n";

    std::cout << "  Reachable Edge Nodes: "
              << accessNode1->GetReachableEdgeNodeCount()
              << "\n";

    for (uint32_t i = 0;
         i < accessNode1->GetReachableEdgeNodeCount();
         ++i)
    {
        Ptr<SixGEdgeNode> edge =
            accessNode1->GetReachableEdgeNode(i);

        std::cout << "    - "
                  << edge->GetNodeId()
                  << " | Compute capacity = "
                  << edge->GetResource("COMPUTING")->GetCapacity()
                  << "\n";
    }

    std::cout << "\nUE-2 topology:\n";
    std::cout << "  UE-2 -> "
              << ue2->GetConnectedAccessNodeObject()->GetNodeId()
              << "\n";

    std::cout << "  Reachable Edge Nodes: "
              << accessNode2->GetReachableEdgeNodeCount()
              << "\n";

    for (uint32_t i = 0;
         i < accessNode2->GetReachableEdgeNodeCount();
         ++i)
    {
        Ptr<SixGEdgeNode> edge =
            accessNode2->GetReachableEdgeNode(i);

        std::cout << "    - "
                  << edge->GetNodeId()
                  << " | Compute capacity = "
                  << edge->GetResource("COMPUTING")->GetCapacity()
                  << "\n";
    }

    // -------------------------------------------------
    // Validate relationships
    // -------------------------------------------------

    std::cout << "\nTopology validation:\n";

    std::cout << "  UE-1 -> AccessNode-1 : "
              << (ue1->GetConnectedAccessNodeObject() == accessNode1
                      ? "VALID"
                      : "INVALID")
              << "\n";

    std::cout << "  UE-2 -> AccessNode-2 : "
              << (ue2->GetConnectedAccessNodeObject() == accessNode2
                      ? "VALID"
                      : "INVALID")
              << "\n";

    std::cout << "  AccessNode-1 -> EdgeNode-1 : "
              << (accessNode1->HasReachableEdgeNode(edgeNode1)
                      ? "VALID"
                      : "INVALID")
              << "\n";

    std::cout << "  AccessNode-1 -> EdgeNode-2 : "
              << (accessNode1->HasReachableEdgeNode(edgeNode2)
                      ? "VALID"
                      : "INVALID")
              << "\n";

    std::cout << "  AccessNode-2 -> EdgeNode-2 : "
              << (accessNode2->HasReachableEdgeNode(edgeNode2)
                      ? "VALID"
                      : "INVALID")
              << "\n";

    std::cout << "  AccessNode-2 -> EdgeNode-3 : "
              << (accessNode2->HasReachableEdgeNode(edgeNode3)
                      ? "VALID"
                      : "INVALID")
              << "\n";

    std::cout << "\n============================================\n";
    std::cout << "   HETEROGENEOUS NETWORK READY\n";
    std::cout << "============================================\n";

    return 0;
}
