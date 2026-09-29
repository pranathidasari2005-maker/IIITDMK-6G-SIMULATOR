#include "ns3/core-module.h"
#include "ns3/network-nodes-module.h"

#include <iostream>
#include <cmath>

using namespace ns3;

int
main(int argc, char* argv[])
{
    double ueX = 50.0;
    double ueY = 0.0;
    double access1X = 0.0;
    double access1Y = 0.0;
    double access2X = 100.0;
    double access2Y = 0.0;

    CommandLine cmd(__FILE__);

    cmd.AddValue("ueX", "UE X position", ueX);
    cmd.AddValue("ueY", "UE Y position", ueY);
    cmd.AddValue("access1X", "AccessNode-1 X position", access1X);
    cmd.AddValue("access1Y", "AccessNode-1 Y position", access1Y);
    cmd.AddValue("access2X", "AccessNode-2 X position", access2X);
    cmd.AddValue("access2Y", "AccessNode-2 Y position", access2Y);

    cmd.Parse(argc, argv);

    std::cout << "\n============================================\n";
    std::cout << "     6G-LENA MOBILITY / ACCESS HANDOVER\n";
    std::cout << "============================================\n";

    Ptr<SixGUe> ue =
        CreateObject<SixGUe>();
    ue->SetNodeId("UE-1");

    Ptr<SixGAccessNode> accessNode1 =
        CreateObject<SixGAccessNode>();
    accessNode1->SetNodeId("AccessNode-1");

    Ptr<SixGAccessNode> accessNode2 =
        CreateObject<SixGAccessNode>();
    accessNode2->SetNodeId("AccessNode-2");

    ue->SetPosition(Vector(ueX, ueY, 0.0));
    accessNode1->SetPosition(Vector(access1X, access1Y, 0.0));
    accessNode2->SetPosition(Vector(access2X, access2Y, 0.0));

    Ptr<SixGEdgeNode> edgeNode1 =
        CreateObject<SixGEdgeNode>();
    edgeNode1->SetNodeId("EdgeNode-1");

    Ptr<SixGEdgeNode> edgeNode2 =
        CreateObject<SixGEdgeNode>();
    edgeNode2->SetNodeId("EdgeNode-2");

    accessNode1->AddReachableEdgeNode(edgeNode1);
    accessNode2->AddReachableEdgeNode(edgeNode2);

    std::cout << "\nInitial topology:\n";
    std::cout << "  UE                  : "
              << ue->GetNodeId() << "\n";
    std::cout << "  Initial Access Node : "
              << accessNode1->GetNodeId() << "\n";

    ue->SetConnectedAccessNode(accessNode1);

    std::cout << "\nInitial connection:\n";
    std::cout << "  Serving Access Node : "
              << ue->GetConnectedAccessNode()
              << "\n";

    bool initialConnection =
        ue->GetConnectedAccessNodeObject() == accessNode1;

    std::cout << "  Connection status   : "
              << (initialConnection ? "VALID" : "INVALID")
              << "\n";

    std::cout << "\n--------------------------------------------\n";
    std::cout << "              MOBILITY EVENT\n";
    std::cout << "--------------------------------------------\n";

    Vector uePosition = ue->GetPosition();
    Vector access1Position = accessNode1->GetPosition();
    Vector access2Position = accessNode2->GetPosition();

    double distanceToAccess1 =
        std::sqrt(
            std::pow(uePosition.x - access1Position.x, 2) +
            std::pow(uePosition.y - access1Position.y, 2));

    double distanceToAccess2 =
        std::sqrt(
            std::pow(uePosition.x - access2Position.x, 2) +
            std::pow(uePosition.y - access2Position.y, 2));

    std::cout << "UE position           : ("
              << uePosition.x << ", "
              << uePosition.y << ")\n";

    std::cout << "Distance to AccessNode-1 : "
              << distanceToAccess1 << "\n";

    std::cout << "Distance to AccessNode-2 : "
              << distanceToAccess2 << "\n";

    SixGAccessNode* targetAccessNode = nullptr;

    if (distanceToAccess2 < distanceToAccess1)
    {
        targetAccessNode = PeekPointer(accessNode2);
    }
    else
    {
        targetAccessNode = PeekPointer(accessNode1);
    }

    bool handoverRequired =
        ue->GetConnectedAccessNodeObject() != targetAccessNode;

    std::cout << "Handover required      : "
              << (handoverRequired ? "YES" : "NO")
              << "\n";

    bool detached = true;

    if (handoverRequired)
    {
        ue->SetConnectedAccessNode(
            Ptr<SixGAccessNode>(nullptr));

        detached =
            ue->GetConnectedAccessNodeObject() == nullptr;

        std::cout << "  Old connection released : "
                  << (detached ? "YES" : "NO")
                  << "\n";

        if (targetAccessNode == PeekPointer(accessNode1))
        {
            ue->SetConnectedAccessNode(accessNode1);
        }
        else
        {
            ue->SetConnectedAccessNode(accessNode2);
        }
    }

    bool handoverSuccessful =
        ue->GetConnectedAccessNodeObject() == targetAccessNode;

    std::cout << "\nHandover result:\n";
    std::cout << "  New Serving Access Node : "
              << ue->GetConnectedAccessNode()
              << "\n";
    std::cout << "  Handover status         : "
              << (handoverSuccessful ? "SUCCESS" : "FAILED")
              << "\n";

    std::cout << "\nPost-handover topology:\n";
    std::cout << "  UE                  : "
              << ue->GetNodeId() << "\n";
    Ptr<SixGAccessNode> servingAccessNode =
        ue->GetConnectedAccessNodeObject();

    std::cout << "  Serving Access Node : "
              << servingAccessNode->GetNodeId() << "\n";
    std::cout << "  Reachable Edge Node : "
              << servingAccessNode->GetReachableEdgeNode(0)->GetNodeId()
              << "\n";

    std::cout << "\n============================================\n";

    if (initialConnection && detached && handoverSuccessful)
    {
        std::cout << "       MOBILITY HANDOVER COMPLETE\n";
    }
    else
    {
        std::cout << "       MOBILITY HANDOVER FAILED\n";
    }

    std::cout << "============================================\n";

    return (initialConnection && detached && handoverSuccessful) ? 0 : 1;
}
