#include "ns3/core-module.h"
#include "ns3/network-nodes-module.h"

#include <iostream>
#include <iomanip>
#include <vector>

using namespace ns3;

int
main(int argc, char* argv[])
{
    double communicationCapacity = 100.0;
    double computingCapacity = 100.0;

    double communicationPerSession = 20.0;
    double computingPerSession = 20.0;

    CommandLine cmd;
    cmd.AddValue("communicationCapacity",
                 "Communication resource capacity",
                 communicationCapacity);
    cmd.AddValue("computingCapacity",
                 "Computing resource capacity",
                 computingCapacity);
    cmd.AddValue("communicationPerSession",
                 "Communication resource required per session",
                 communicationPerSession);
    cmd.AddValue("computingPerSession",
                 "Computing resource required per session",
                 computingPerSession);
    cmd.Parse(argc, argv);

    std::cout << "\n============================================\n";
    std::cout << "  6G-LENA COMMUNICATION + COMPUTING LOAD EXPERIMENT\n";
    std::cout << "============================================\n";


    Ptr<SixGAccessNode> accessNode = CreateObject<SixGAccessNode>();
    Ptr<SixGEdgeNode> edgeNode = CreateObject<SixGEdgeNode>();

    accessNode->AddReachableEdgeNode(edgeNode);

    auto communication = accessNode->GetResource("COMMUNICATION");
    auto computing = edgeNode->GetResource("COMPUTING");

    communication->SetCapacity(communicationCapacity);
    communication->SetAvailable(communicationCapacity);

    computing->SetCapacity(computingCapacity);
    computing->SetAvailable(computingCapacity);

    std::cout << std::fixed << std::setprecision(2);

    std::cout << "\nResource configuration:\n";
    std::cout << "  Communication capacity : "
              << communicationCapacity << "\n";
    std::cout << "  Computing capacity     : "
              << computingCapacity << "\n";
    std::cout << "  Communication/session  : "
              << communicationPerSession << "\n";
    std::cout << "  Computing/session      : "
              << computingPerSession << "\n";

    std::cout << "\nload_offered,successful,failed,success_rate,"
                 "communication_utilization,computing_utilization\n";

    for (uint32_t offeredLoad = 1; offeredLoad <= 8; ++offeredLoad)
    {
        std::vector<Ptr<SixGServiceSession>> activeSessions;

        uint32_t successful = 0;
        uint32_t failed = 0;

        for (uint32_t i = 0; i < offeredLoad; ++i)
        {
            Ptr<SixGUe> ue = CreateObject<SixGUe>();
            Ptr<SixGService> service = CreateObject<SixGService>();

            service->SetActive(true);
            service->SetRequiredCommunication(communicationPerSession);
            service->SetRequiredCompute(computingPerSession);

            ue->SetConnectedAccessNode(accessNode);

            Ptr<SixGServiceSession> session =
                CreateObject<SixGServiceSession>();

            session->SetUe(ue);
            session->SetAccessNode(accessNode);
            session->SetEdgeNode(edgeNode);
            session->SetService(service);

            if (session->Activate())
            {
                activeSessions.push_back(session);
                successful++;
            }
            else
            {
                failed++;
            }
        }

        const double communicationUtilization =
            100.0 *
            (communicationCapacity - communication->GetAvailable()) /
            communicationCapacity;

        const double computingUtilization =
            100.0 *
            (computingCapacity - computing->GetAvailable()) /
            computingCapacity;

        const double successRate =
            100.0 * static_cast<double>(successful) /
            static_cast<double>(offeredLoad);

        std::cout << offeredLoad << ","
                  << successful << ","
                  << failed << ","
                  << successRate << ","
                  << communicationUtilization << ","
                  << computingUtilization << "\n";

        for (auto session : activeSessions)
        {
            session->Deactivate();
        }

        const bool resourcesRestored =
            communication->GetAvailable() == communicationCapacity &&
            computing->GetAvailable() == computingCapacity;

        if (!resourcesRestored)
        {
            std::cerr << "ERROR: resources were not fully restored "
                         "after load level "
                      << offeredLoad << "\n";
            return 1;
        }
    }

    std::cout << "\nFinal resource state:\n";
    std::cout << "  Communication available: "
              << communication->GetAvailable() << "\n";
    std::cout << "  Computing available    : "
              << computing->GetAvailable() << "\n";

    std::cout << "\n============================================\n";
    std::cout << " COMMUNICATION + COMPUTING LOAD EXPERIMENT PASSED\n";
    std::cout << "============================================\n";

    return 0;
}
