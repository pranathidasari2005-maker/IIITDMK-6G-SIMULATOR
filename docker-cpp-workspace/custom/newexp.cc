
#include "ns3/core-module.h"
#include "ns3/network-nodes-module.h"

#include <iostream>
#include <vector>

using namespace ns3;

int
main(int argc, char* argv[])
{
    std::cout << "\n========================================\n";
    std::cout << " CUSTOM 6G HETEROGENEOUS NETWORK\n";
    std::cout << "========================================\n";

    // Network configuration
    const uint32_t accessCount = 2;
    const uint32_t edgeCount = 2;
    const uint32_t ueCount = 4;

    std::vector<Ptr<SixGAccessNode>> accessNodes;
    std::vector<Ptr<SixGEdgeNode>> edgeNodes;
    std::vector<Ptr<SixGUe>> ues;

    // 1. Create edge computing nodes
    for (uint32_t i = 0; i < edgeCount; ++i)
    {
        Ptr<SixGEdgeNode> edge = CreateObject<SixGEdgeNode>();

        edge->SetNodeId("Edge-" + std::to_string(i + 1));
        edge->SetPosition(Vector(50.0 * i, 20.0, 0.0));
        edge->SetComputeCapacity(100.0);
        edge->SetAvailableCompute(100.0);
        edge->SetMemoryCapacity(512.0);
        edge->SetAvailableMemory(512.0);
        edge->Initialize();

        edgeNodes.push_back(edge);
    }

    // 2. Create access nodes
    for (uint32_t i = 0; i < accessCount; ++i)
    {
        Ptr<SixGAccessNode> access =
            CreateObject<SixGAccessNode>();

        access->SetNodeId("Access-" + std::to_string(i + 1));
        access->SetPosition(Vector(100.0 * i, 0.0, 0.0));
        access->SetCommunicationCapacity(1000.0);
        access->SetAvailableCommunication(1000.0);
        access->SetCoverageRadius(100.0);
        access->Initialize();

        // Connect each access node to both edge nodes
        for (const auto& edge : edgeNodes)
        {
            access->AddReachableEdgeNode(edge);
        }

        accessNodes.push_back(access);
    }

    // 3. Create UEs and associate them with access nodes
    for (uint32_t i = 0; i < ueCount; ++i)
    {
        Ptr<SixGUe> ue = CreateObject<SixGUe>();

        ue->SetNodeId("UE-" + std::to_string(i + 1));
        ue->SetPosition(Vector(10.0 * i, 5.0, 0.0));

        Ptr<SixGAccessNode> servingAccess =
            accessNodes[i % accessNodes.size()];

        ue->SetConnectedAccessNode(servingAccess);
        ue->SetConnected(true);

        servingAccess->AddConnectedUe(ue->GetNodeId());

        ue->Initialize();
        ues.push_back(ue);
    }

    // 4. Display network configuration
    std::cout << "\nNETWORK CONFIGURATION\n";
    std::cout << "Access Nodes : " << accessNodes.size() << "\n";
    std::cout << "Edge Nodes   : " << edgeNodes.size() << "\n";
    std::cout << "UEs          : " << ues.size() << "\n";

    // 5. Display edge node resources
    std::cout << "\nEDGE NODE RESOURCES\n";

    for (const auto& edge : edgeNodes)
    {
        std::cout << edge->GetNodeId()
                  << " | Compute: " << edge->GetAvailableCompute()
                  << "/" << edge->GetComputeCapacity()
                  << " | Memory: " << edge->GetAvailableMemory()
                  << "/" << edge->GetMemoryCapacity()
                  << "\n";
    }

    // 6. Display access node associations
    std::cout << "\nACCESS NODE ASSOCIATIONS\n";

    for (const auto& access : accessNodes)
    {
        std::cout << access->GetNodeId()
                  << " | Connected UEs: "
                  << access->GetConnectedUeCount()
                  << " | Reachable Edge Nodes: "
                  << access->GetReachableEdgeNodeCount()
                  << "\n";
    }

    // 7. Validate UE connectivity
    std::cout << "\nUE CONNECTIVITY\n";

    bool networkValid = true;

    for (const auto& ue : ues)
    {
        Ptr<SixGAccessNode> servingAccess =
            ue->GetConnectedAccessNodeObject();

        bool valid =
            ue->IsConnected() &&
            servingAccess != nullptr &&
            servingAccess->GetReachableEdgeNodeCount() > 0 &&
            servingAccess->HasConnectedUe(ue->GetNodeId());

        std::cout << ue->GetNodeId() << " -> ";

        if (servingAccess != nullptr)
        {
            std::cout << servingAccess->GetNodeId();
        }
        else
        {
            std::cout << "No Access Node";
        }

        std::cout << " : " << (valid ? "VALID" : "INVALID") << "\n";

        if (!valid)
        {
            networkValid = false;
        }
    }

    std::cout << "\n========================================\n";

    if (networkValid)
    {
        std::cout << " NETWORK VALIDATION SUCCESSFUL\n";
    }
    else
    {
        std::cout << " NETWORK VALIDATION FAILED\n";
    }

    std::cout << "========================================\n";

    return networkValid ? 0 : 1;
}
