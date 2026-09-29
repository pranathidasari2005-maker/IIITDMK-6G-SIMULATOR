#include "ns3/6g-capability-registry.h"
#include "ns3/6g-capability.h"
#include "ns3/node.h"
#include "ns3/node-container.h"
#include "ns3/test.h"

using namespace ns3;

/**
 * \brief Tests registration and capability counting.
 */
class SixGCapabilityRegistryRegistrationTestCase : public TestCase
{
  public:
    SixGCapabilityRegistryRegistrationTestCase()
        : TestCase("Register capabilities and verify count")
    {
    }

  private:
    void DoRun() override;
};

void
SixGCapabilityRegistryRegistrationTestCase::DoRun()
{
    Ptr<Node> node = CreateObject<Node>();

    Ptr<SixGCapabilityRegistry> registry =
        CreateObject<SixGCapabilityRegistry>();

    Ptr<SixGCapability> capability =
        CreateObject<SixGCapability>();

    capability->SetCapabilityName("Computing");
    capability->SetCapabilityType(SixGCapability::COMPUTING);

    registry->RegisterCapability(node, capability);

    NS_TEST_EXPECT_MSG_EQ(registry->GetCapabilityCount(),
                          1,
                          "Registry should contain one capability");

    auto registrations = registry->GetRegistrations();

    NS_TEST_EXPECT_MSG_EQ(registrations.size(),
                          1,
                          "GetRegistrations should return one entry");

    NS_TEST_EXPECT_MSG_EQ(registrations[0].node->GetId(),
                          node->GetId(),
                          "Registered provider node is incorrect");

    NS_TEST_EXPECT_MSG_EQ(registrations[0].capability->GetCapabilityName(),
                          "Computing",
                          "Registered capability name is incorrect");
}

/**
 * \brief Tests capability type discovery.
 */
class SixGCapabilityRegistryDiscoveryTestCase : public TestCase
{
  public:
    SixGCapabilityRegistryDiscoveryTestCase()
        : TestCase("Find capabilities by type")
    {
    }

  private:
    void DoRun() override;
};

void
SixGCapabilityRegistryDiscoveryTestCase::DoRun()
{
    NodeContainer nodes;
    nodes.Create(3);

    Ptr<SixGCapabilityRegistry> registry =
        CreateObject<SixGCapabilityRegistry>();

    Ptr<SixGCapability> communication =
        CreateObject<SixGCapability>();
    communication->SetCapabilityName("Communication");
    communication->SetCapabilityType(
        SixGCapability::COMMUNICATION);

    Ptr<SixGCapability> computing =
        CreateObject<SixGCapability>();
    computing->SetCapabilityName("Computing");
    computing->SetCapabilityType(
        SixGCapability::COMPUTING);

    Ptr<SixGCapability> sensing =
        CreateObject<SixGCapability>();
    sensing->SetCapabilityName("Sensing");
    sensing->SetCapabilityType(
        SixGCapability::SENSING);

    registry->RegisterCapability(nodes.Get(0), communication);
    registry->RegisterCapability(nodes.Get(1), computing);
    registry->RegisterCapability(nodes.Get(2), sensing);

    auto computingResults =
        registry->FindCapabilitiesByType(
            SixGCapability::COMPUTING);

    NS_TEST_EXPECT_MSG_EQ(computingResults.size(),
                          1,
                          "Exactly one computing capability should be found");

    NS_TEST_EXPECT_MSG_EQ(computingResults[0].node->GetId(),
                          nodes.Get(1)->GetId(),
                          "Computing capability provider is incorrect");

    NS_TEST_EXPECT_MSG_EQ(
        computingResults[0].capability->GetCapabilityName(),
        "Computing",
        "Wrong capability returned for computing query");

    auto sensingResults =
        registry->FindCapabilitiesByType(
            SixGCapability::SENSING);

    NS_TEST_EXPECT_MSG_EQ(sensingResults.size(),
                          1,
                          "Exactly one sensing capability should be found");

    NS_TEST_EXPECT_MSG_EQ(sensingResults[0].node->GetId(),
                          nodes.Get(2)->GetId(),
                          "Sensing capability provider is incorrect");
}

/**
 * \brief Tests capability removal.
 */
class SixGCapabilityRegistryRemovalTestCase : public TestCase
{
  public:
    SixGCapabilityRegistryRemovalTestCase()
        : TestCase("Remove registered capability")
    {
    }

  private:
    void DoRun() override;
};

void
SixGCapabilityRegistryRemovalTestCase::DoRun()
{
    Ptr<Node> node = CreateObject<Node>();

    Ptr<SixGCapabilityRegistry> registry =
        CreateObject<SixGCapabilityRegistry>();

    Ptr<SixGCapability> capability =
        CreateObject<SixGCapability>();

    capability->SetCapabilityName("Sensing");
    capability->SetCapabilityType(SixGCapability::SENSING);

    registry->RegisterCapability(node, capability);

    NS_TEST_EXPECT_MSG_EQ(registry->GetCapabilityCount(),
                          1,
                          "Capability should be registered");

    bool removed =
        registry->RemoveCapability(node, capability);

    NS_TEST_EXPECT_MSG_EQ(removed,
                          true,
                          "Registered capability should be removable");

    NS_TEST_EXPECT_MSG_EQ(registry->GetCapabilityCount(),
                          0,
                          "Registry should be empty after removal");

    bool removedAgain =
        registry->RemoveCapability(node, capability);

    NS_TEST_EXPECT_MSG_EQ(removedAgain,
                          false,
                          "Removing an already removed capability should fail");
}

/**
 * \brief Tests handling of invalid registration input.
 */
class SixGCapabilityRegistryInvalidInputTestCase : public TestCase
{
  public:
    SixGCapabilityRegistryInvalidInputTestCase()
        : TestCase("Reject null registration input")
    {
    }

  private:
    void DoRun() override;
};

void
SixGCapabilityRegistryInvalidInputTestCase::DoRun()
{
    Ptr<SixGCapabilityRegistry> registry =
        CreateObject<SixGCapabilityRegistry>();

    Ptr<Node> node = CreateObject<Node>();
    Ptr<SixGCapability> capability =
        CreateObject<SixGCapability>();

    registry->RegisterCapability(nullptr, capability);
    registry->RegisterCapability(node, nullptr);

    NS_TEST_EXPECT_MSG_EQ(registry->GetCapabilityCount(),
                          0,
                          "Null registration input must not be stored");
}

/**
 * \brief SixGCapabilityRegistry TestSuite.
 */
class SixGCapabilityRegistryTestSuite : public TestSuite
{
  public:
    SixGCapabilityRegistryTestSuite()
        : TestSuite("6g-capability-registry", Type::UNIT)
    {
        AddTestCase(
            new SixGCapabilityRegistryRegistrationTestCase(),
            TestCase::Duration::QUICK);

        AddTestCase(
            new SixGCapabilityRegistryDiscoveryTestCase(),
            TestCase::Duration::QUICK);

        AddTestCase(
            new SixGCapabilityRegistryRemovalTestCase(),
            TestCase::Duration::QUICK);

        AddTestCase(
            new SixGCapabilityRegistryInvalidInputTestCase(),
            TestCase::Duration::QUICK);
    }
};

static SixGCapabilityRegistryTestSuite
    g_sixGCapabilityRegistryTestSuiteInstance;
