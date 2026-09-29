#include "ns3/6g-resource.h"
#include "ns3/6g-network-node.h"
#include "ns3/test.h"

#include <cmath>

namespace ns3
{

class SixGResourceTestCase : public TestCase
{
  public:
    SixGResourceTestCase()
        : TestCase("Test generic 6G resource lifecycle")
    {
    }

  private:
    void DoRun() override
    {
        Ptr<SixGResource> resource = CreateObject<SixGResource>();

        // Initial state
        resource->SetType("COMPUTING");
        NS_TEST_ASSERT_MSG_EQ(resource->GetType(), "COMPUTING", "Resource type mismatch");

        NS_TEST_ASSERT_MSG_EQ(resource->SetCapacity(100.0),
                              true,
                              "Capacity should be accepted");

        NS_TEST_ASSERT_MSG_EQ(resource->SetAvailable(100.0),
                              true,
                              "Available resource should be accepted");

        NS_TEST_ASSERT_MSG_EQ(resource->GetCapacity(),
                              100.0,
                              "Capacity mismatch");

        NS_TEST_ASSERT_MSG_EQ(resource->GetAvailable(),
                              100.0,
                              "Available resource mismatch");

        NS_TEST_ASSERT_MSG_EQ(resource->GetUtilization(),
                              0.0,
                              "Initial utilization should be zero");

        // Consume resource
        NS_TEST_ASSERT_MSG_EQ(resource->Consume(40.0),
                              true,
                              "Valid consumption should succeed");

        NS_TEST_ASSERT_MSG_EQ(resource->GetAvailable(),
                              60.0,
                              "Available resource should become 60");

        NS_TEST_ASSERT_MSG_EQ_TOL(resource->GetUtilization(),
                                  0.40,
                                  1e-9,
                                  "Utilization should be 40%");

        // Reject over-consumption
        NS_TEST_ASSERT_MSG_EQ(resource->Consume(70.0),
                              false,
                              "Over-consumption should fail");

        NS_TEST_ASSERT_MSG_EQ(resource->GetAvailable(),
                              60.0,
                              "Failed consumption must not change state");

        // Release resource
        NS_TEST_ASSERT_MSG_EQ(resource->Release(20.0),
                              true,
                              "Valid release should succeed");

        NS_TEST_ASSERT_MSG_EQ(resource->GetAvailable(),
                              80.0,
                              "Available resource should become 80");

        // Reject over-release
        NS_TEST_ASSERT_MSG_EQ(resource->Release(30.0),
                              false,
                              "Over-release should fail");

        NS_TEST_ASSERT_MSG_EQ(resource->GetAvailable(),
                              80.0,
                              "Failed release must not change state");

        // Direct availability update
        NS_TEST_ASSERT_MSG_EQ(resource->SetAvailable(50.0),
                              true,
                              "Valid availability update should succeed");

        NS_TEST_ASSERT_MSG_EQ(resource->GetAvailable(),
                              50.0,
                              "Availability update failed");

        // Invalid values
        NS_TEST_ASSERT_MSG_EQ(resource->SetAvailable(-1.0),
                              false,
                              "Negative availability should fail");

        NS_TEST_ASSERT_MSG_EQ(resource->SetAvailable(101.0),
                              false,
                              "Availability above capacity should fail");

        NS_TEST_ASSERT_MSG_EQ(resource->SetCapacity(-1.0),
                              false,
                              "Negative capacity should fail");

        NS_TEST_ASSERT_MSG_EQ(resource->Consume(-1.0),
                              false,
                              "Negative consumption should fail");

        NS_TEST_ASSERT_MSG_EQ(resource->Release(-1.0),
                              false,
                              "Negative release should fail");

        NS_TEST_ASSERT_MSG_EQ(resource->GetAvailable(),
                              50.0,
                              "Invalid operations must not corrupt state");
    }
};


class SixGNodeResourceRegistryTestCase : public TestCase
{
  public:
    SixGNodeResourceRegistryTestCase()
        : TestCase("Test generic node resource registry")
    {
    }

  private:
    void DoRun() override
    {
        Ptr<SixGNode> node = CreateObject<SixGNode>();

        Ptr<SixGResource> computing = CreateObject<SixGResource>();
        computing->SetType("COMPUTING");
        computing->SetCapacity(100.0);
        computing->SetAvailable(100.0);

        NS_TEST_ASSERT_MSG_EQ(node->AddResource(computing),
                              true,
                              "First resource should be added");

        NS_TEST_ASSERT_MSG_EQ(node->HasResource("COMPUTING"),
                              true,
                              "Resource should be discoverable");

        NS_TEST_ASSERT_MSG_EQ(node->GetResource("COMPUTING"),
                              computing,
                              "Retrieved resource should match added resource");

        Ptr<SixGResource> duplicate = CreateObject<SixGResource>();
        duplicate->SetType("COMPUTING");

        NS_TEST_ASSERT_MSG_EQ(node->AddResource(duplicate),
                              false,
                              "Duplicate resource type should be rejected");

        NS_TEST_ASSERT_MSG_EQ(node->GetResources().size(),
                              1u,
                              "Registry should contain one resource");

        NS_TEST_ASSERT_MSG_EQ(node->RemoveResource("COMPUTING"),
                              true,
                              "Existing resource should be removable");

        NS_TEST_ASSERT_MSG_EQ(node->HasResource("COMPUTING"),
                              false,
                              "Removed resource should no longer be discoverable");

        NS_TEST_ASSERT_MSG_EQ(node->GetResource("COMPUTING"),
                              nullptr,
                              "Removed resource lookup should return null");

        NS_TEST_ASSERT_MSG_EQ(node->RemoveResource("COMPUTING"),
                              false,
                              "Removing missing resource should fail");
    }
};

class SixGResourceTestSuite : public TestSuite
{
  public:
    SixGResourceTestSuite()
        : TestSuite("6g-resource", Type::UNIT)
    {
        AddTestCase(new SixGResourceTestCase);
        AddTestCase(new SixGNodeResourceRegistryTestCase);
    }
};

static SixGResourceTestSuite g_sixGResourceTestSuite;

} // namespace ns3
