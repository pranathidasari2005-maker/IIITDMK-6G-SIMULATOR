#include "ns3/core-module.h"
#include "ns3/network-nodes-module.h"

#include <iostream>
#include <cmath>

using namespace ns3;

int
main(int argc, char* argv[])
{
    double capacity = 100.0;
    double consumption = 70.0;
    double release = 40.0;

    CommandLine cmd(__FILE__);

    cmd.AddValue(
        "capacity",
        "Initial computing resource capacity",
        capacity);

    cmd.AddValue(
        "consumption",
        "Amount of computing resource to consume",
        consumption);

    cmd.AddValue(
        "release",
        "Amount of computing resource to release",
        release);

    cmd.Parse(argc, argv);

    if (capacity <= 0.0)
    {
        std::cerr << "Capacity must be greater than zero.\n";
        return 1;
    }

    if (consumption < 0.0 || consumption > capacity)
    {
        std::cerr
            << "Consumption must be between 0 and capacity.\n";
        return 1;
    }

    if (release < 0.0 || release > consumption)
    {
        std::cerr
            << "Release must be between 0 and the consumed amount.\n";
        return 1;
    }

    std::cout << "\n============================================\n";
    std::cout << "      6G-LENA DYNAMIC RESOURCE STATE\n";
    std::cout << "============================================\n";

    Ptr<SixGEdgeNode> edgeNode =
        CreateObject<SixGEdgeNode>();

    edgeNode->SetNodeId("EdgeNode-1");

    Ptr<SixGResource> compute =
        edgeNode->GetResource("COMPUTING");

    compute->SetCapacity(capacity);
    compute->SetAvailable(capacity);

    const double initialAvailable =
        compute->GetAvailable();

    std::cout << "\nInitial resource state:\n";
    std::cout << "  Node              : "
              << edgeNode->GetNodeId() << "\n";

    std::cout << "  Resource          : COMPUTING\n";

    std::cout << "  Capacity          : "
              << compute->GetCapacity() << "\n";

    std::cout << "  Available         : "
              << compute->GetAvailable() << "\n";

    std::cout << "  Utilization       : "
              << compute->GetUtilization() << "\n";

    std::cout << "\n--------------------------------------------\n";
    std::cout << "              RESOURCE CONSUMPTION\n";
    std::cout << "--------------------------------------------\n";

    bool consumed =
        compute->Consume(consumption);

    std::cout << "Consume "
              << consumption
              << " compute units : "
              << (consumed ? "SUCCESS" : "FAILED")
              << "\n";

    std::cout << "Available after consume  : "
              << compute->GetAvailable()
              << "\n";

    std::cout << "Utilization after consume: "
              << compute->GetUtilization()
              << "\n";

    std::cout << "\nCapability state after consumption:\n";

    std::cout << "  COMPUTING available    : "
              << compute->GetAvailable()
              << "\n";

    std::cout << "  Capability usable      : "
              << (compute->GetAvailable() > 0.0
                      ? "YES"
                      : "NO")
              << "\n";

    std::cout << "\n--------------------------------------------\n";
    std::cout << "              RESOURCE RELEASE\n";
    std::cout << "--------------------------------------------\n";

    bool released =
        compute->Release(release);

    std::cout << "Release "
              << release
              << " compute units : "
              << (released ? "SUCCESS" : "FAILED")
              << "\n";

    std::cout << "Available after release  : "
              << compute->GetAvailable()
              << "\n";

    std::cout << "Utilization after release: "
              << compute->GetUtilization()
              << "\n";

    const double expectedFinalAvailable =
        initialAvailable - consumption + release;

    const double finalAvailable =
        compute->GetAvailable();

    const bool finalStateCorrect =
        std::abs(
            finalAvailable -
            expectedFinalAvailable) < 1e-9;

    std::cout << "\nFinal resource state:\n";

    std::cout << "  Capacity          : "
              << compute->GetCapacity()
              << "\n";

    std::cout << "  Available         : "
              << finalAvailable
              << "\n";

    std::cout << "  Expected available: "
              << expectedFinalAvailable
              << "\n";

    std::cout << "  Final state check : "
              << (finalStateCorrect
                      ? "PASS"
                      : "FAIL")
              << "\n";

    const bool valid =
        consumed &&
        released &&
        finalStateCorrect;

    std::cout << "\n============================================\n";

    if (valid)
    {
        std::cout
            << "      DYNAMIC RESOURCE STATE COMPLETE\n";
    }
    else
    {
        std::cout
            << "      DYNAMIC RESOURCE STATE FAILED\n";
    }

    std::cout << "============================================\n";

    return valid ? 0 : 1;
}
