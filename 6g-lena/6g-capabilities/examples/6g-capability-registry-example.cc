#include "ns3/6g-capability-registry.h"
#include "ns3/6g-capability.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"

#include <iostream>
#include <string>
#include <vector>

using namespace ns3;

static bool
ParseCapabilityType(const std::string& name,
                    SixGCapability::CapabilityType& type)
{
    if (name == "COMMUNICATION")
    {
        type = SixGCapability::COMMUNICATION;
        return true;
    }

    if (name == "SENSING")
    {
        type = SixGCapability::SENSING;
        return true;
    }

    if (name == "COMPUTING")
    {
        type = SixGCapability::COMPUTING;
        return true;
    }

    if (name == "AI")
    {
        type = SixGCapability::AI;
        return true;
    }

    return false;
}

static std::string
CapabilityTypeToName(SixGCapability::CapabilityType type)
{
    switch (type)
    {
    case SixGCapability::COMMUNICATION:
        return "Communication";

    case SixGCapability::SENSING:
        return "Sensing";

    case SixGCapability::COMPUTING:
        return "Computing";

    case SixGCapability::AI:
        return "AI";

    default:
        return "Unknown";
    }
}

int
main(int argc, char* argv[])
{
    uint32_t communicationProviders = 1;
    uint32_t sensingProviders = 1;
    uint32_t computingProviders = 1;
    uint32_t aiProviders = 1;
    std::string queryType = "COMPUTING";

    CommandLine cmd(__FILE__);

    cmd.AddValue("communicationProviders",
                 "Number of communication capability providers",
                 communicationProviders);

    cmd.AddValue("sensingProviders",
                 "Number of sensing capability providers",
                 sensingProviders);

    cmd.AddValue("computingProviders",
                 "Number of computing capability providers",
                 computingProviders);

    cmd.AddValue("aiProviders",
                 "Number of AI capability providers",
                 aiProviders);

    cmd.AddValue("queryType",
                 "Capability discovery query: COMMUNICATION, SENSING, COMPUTING, or AI",
                 queryType);

    cmd.Parse(argc, argv);

    SixGCapability::CapabilityType queryCapabilityType;

    if (!ParseCapabilityType(queryType, queryCapabilityType))
    {
        std::cerr << "ERROR: Invalid queryType '" << queryType << "'\n";
        std::cerr << "Valid values: COMMUNICATION, SENSING, COMPUTING, AI\n";
        return 1;
    }

    uint32_t totalProviders =
        communicationProviders +
        sensingProviders +
        computingProviders +
        aiProviders;

    if (totalProviders == 0)
    {
        std::cerr << "ERROR: At least one capability provider is required.\n";
        return 1;
    }

    std::cout << "========================================\n";
    std::cout << "       6G CAPABILITY REGISTRY\n";
    std::cout << "========================================\n\n";

    std::cout << "Registry configuration:\n";
    std::cout << "  Communication providers : "
              << communicationProviders << "\n";
    std::cout << "  Sensing providers       : "
              << sensingProviders << "\n";
    std::cout << "  Computing providers     : "
              << computingProviders << "\n";
    std::cout << "  AI providers            : "
              << aiProviders << "\n";
    std::cout << "  Discovery query         : "
              << queryType << "\n";
    std::cout << "  Total providers         : "
              << totalProviders << "\n\n";

    // Create one network node for every configured provider.
    NodeContainer nodes;
    nodes.Create(totalProviders);

    // Create the global capability registry.
    Ptr<SixGCapabilityRegistry> registry =
        CreateObject<SixGCapabilityRegistry>();

    uint32_t nodeIndex = 0;

    auto registerProviders =
        [&](uint32_t count,
            SixGCapability::CapabilityType type)
    {
        for (uint32_t i = 0; i < count; ++i)
        {
            Ptr<SixGCapability> capability =
                CreateObject<SixGCapability>();

            capability->SetCapabilityName(
                CapabilityTypeToName(type));

            capability->SetCapabilityType(type);

            registry->RegisterCapability(
                nodes.Get(nodeIndex),
                capability);

            ++nodeIndex;
        }
    };

    registerProviders(
        communicationProviders,
        SixGCapability::COMMUNICATION);

    registerProviders(
        sensingProviders,
        SixGCapability::SENSING);

    registerProviders(
        computingProviders,
        SixGCapability::COMPUTING);

    registerProviders(
        aiProviders,
        SixGCapability::AI);

    // ------------------------------------------------------------
    // Display registered capabilities
    // ------------------------------------------------------------

    std::cout << "Registered capabilities:\n\n";

    for (const auto& registration : registry->GetRegistrations())
    {
        std::cout << "Node "
                  << registration.node->GetId()
                  << " -> "
                  << registration.capability->GetCapabilityName()
                  << " : "
                  << (registration.capability->IsAvailable()
                          ? "AVAILABLE"
                          : "UNAVAILABLE")
                  << "\n";
    }

    // ------------------------------------------------------------
    // Discovery query
    // ------------------------------------------------------------

    std::cout << "\n----------------------------------------\n";
    std::cout << "Discovery Query: "
              << queryType
              << "\n";
    std::cout << "----------------------------------------\n\n";

    auto matchingCapabilities =
        registry->FindCapabilitiesByType(
            queryCapabilityType);

    std::cout << "Found:\n";

    for (const auto& registration : matchingCapabilities)
    {
        std::cout << "  Node "
                  << registration.node->GetId()
                  << " -> "
                  << registration.capability->GetCapabilityName()
                  << "\n";
    }

    std::cout << "\n----------------------------------------\n";
    std::cout << "Total registered capabilities: "
              << registry->GetCapabilityCount()
              << "\n";
    std::cout << "Matching capabilities: "
              << matchingCapabilities.size()
              << "\n";
    std::cout << "========================================\n";

    return 0;
}
