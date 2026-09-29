#include "ns3/core-module.h"
#include "ns3/network-nodes-module.h"

#include <iostream>
#include <vector>

using namespace ns3;

int
main(int argc, char* argv[])
{
    uint32_t accessNodeCount = 1;
    uint32_t edgeNodeCount = 1;
    uint32_t ueCount = 1;
    uint32_t serviceCount = 0;

    CommandLine cmd(__FILE__);

    cmd.AddValue(
        "accessNodes",
        "Number of 6G access nodes",
        accessNodeCount);

    cmd.AddValue(
        "edgeNodes",
        "Number of 6G edge nodes",
        edgeNodeCount);

    cmd.AddValue(
        "ues",
        "Number of user equipment nodes",
        ueCount);

    cmd.AddValue(
        "services",
        "Number of configured services",
        serviceCount);

    cmd.Parse(argc, argv);

    if (accessNodeCount < 1 ||
        edgeNodeCount < 1 ||
        ueCount < 1)
    {
        std::cerr
            << "ERROR: accessNodes, edgeNodes and ues "
            << "must all be at least 1.\n";

        return 1;
    }

    std::cout << "\n============================================\n";
    std::cout << "       6G-LENA BASIC NETWORK EXAMPLE\n";
    std::cout << "============================================\n";

    std::vector<Ptr<SixGAccessNode>> accessNodes;
    std::vector<Ptr<SixGEdgeNode>> edgeNodes;
    std::vector<Ptr<SixGUe>> ues;

    /*
     * Create EDGE nodes.
     */
    for (uint32_t i = 0; i < edgeNodeCount; ++i)
    {
        Ptr<SixGEdgeNode> edgeNode =
            CreateObject<SixGEdgeNode>();

        edgeNode->SetNodeId(
            "EdgeNode-" + std::to_string(i + 1));

        edgeNodes.push_back(edgeNode);
    }

    /*
     * Create ACCESS nodes and connect
     * every access node to every edge node.
     */
    for (uint32_t i = 0; i < accessNodeCount; ++i)
    {
        Ptr<SixGAccessNode> accessNode =
            CreateObject<SixGAccessNode>();

        accessNode->SetNodeId(
            "AccessNode-" + std::to_string(i + 1));

        for (const auto& edgeNode : edgeNodes)
        {
            accessNode->AddReachableEdgeNode(edgeNode);
        }

        accessNodes.push_back(accessNode);
    }

    /*
     * Create UEs and distribute them across
     * the available access nodes.
     */
    for (uint32_t i = 0; i < ueCount; ++i)
    {
        Ptr<SixGUe> ue =
            CreateObject<SixGUe>();

        ue->SetNodeId(
            "UE-" + std::to_string(i + 1));

        Ptr<SixGAccessNode> servingAccessNode =
            accessNodes[i % accessNodes.size()];

        ue->SetConnectedAccessNode(
            servingAccessNode);

        ues.push_back(ue);
    }

    /*
     * Simulation summary.
     */
    std::cout << "Access Nodes        : "
              << accessNodes.size() << "\n";

    std::cout << "Edge Nodes          : "
              << edgeNodes.size() << "\n";

    std::cout << "UEs                 : "
              << ues.size() << "\n";

    std::cout << "Services            : "
              << serviceCount << "\n";

    std::cout << "\n";

    /*
     * Display each access node and its
     * reachable edge nodes.
     */
    for (uint32_t i = 0;
         i < accessNodes.size();
         ++i)
    {
        std::cout
            << accessNodes[i]->GetNodeId()
            << " -> "
            << accessNodes[i]->GetReachableEdgeNodeCount()
            << " reachable edge nodes\n";
    }

    std::cout << "\n";

    /*
     * Display each UE and its serving access node.
     */
    for (const auto& ue : ues)
    {
        Ptr<SixGAccessNode> servingAccess =
            ue->GetConnectedAccessNodeObject();

        std::cout
            << ue->GetNodeId()
            << " -> "
            << servingAccess->GetNodeId()
            << " : "
            << (
                servingAccess != nullptr
                    ? "VALID"
                    : "INVALID"
            )
            << "\n";
    }

    /*
     * Validate the network.
     */
    bool networkValid = true;

    for (const auto& ue : ues)
    {
        Ptr<SixGAccessNode> servingAccess =
            ue->GetConnectedAccessNodeObject();

        if (servingAccess == nullptr)
        {
            networkValid = false;
            break;
        }

        if (servingAccess->GetReachableEdgeNodeCount() == 0)
        {
            networkValid = false;
            break;
        }
    }

    std::cout << "\n============================================\n";

    if (networkValid)
    {
        std::cout
            << "        BASIC NETWORK READY\n";
    }
    else
    {
        std::cout
            << "        NETWORK VALIDATION FAILED\n";
    }

    std::cout << "============================================\n";

    return networkValid ? 0 : 1;
}
