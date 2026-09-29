#include "ns3/6g-resource.h"
#include "ns3/test.h"

#include <cmath>

using namespace ns3;

class SixGResourceTestCase : public TestCase
{
  public:
    SixGResourceTestCase()
        : TestCase("SixGResource basic resource lifecycle")
    {
    }

  private:
    void DoRun() override
    {
        Ptr<SixGResource> resource = CreateObject<SixGResource>();

        // Initial state
        NS_TEST_ASSERT_MSG_EQ(resource->GetType(), "", "Initial type should be empty");
        NS_TEST_ASSERT_MSG_EQ(resource->GetCapacity(), 0.0, "Initial capacity should be zero");
        NS_TEST_ASSERT_MSG_EQ(resource->GetAvailable(), 0.0, "Initial availability should be zero");
        NS_TEST_ASSERT_MSG_EQ(resource->GetUtilization(), 0.0, "Initial utilization should be zero");

        // Configure resource
        resource->SetType("COMPUTING");
        NS_TEST_ASSERT_MSG_EQ(resource->GetType(), "COMPUTING", "Resource type mismatch");

        NS_TEST_ASSERT_MSG_EQ(resource->SetCapacity(100.0),
                              true,
                              "Valid capacity should be accepted");

        NS_TEST_ASSERT_MSG_EQ(resource->SetAvailable(100.0),
                              true,
                              "Valid availability should be accepted");

        NS_TEST_ASSERT_MSG_EQ(resource->GetCapacity(),
                              100.0,
                              "Capacity should be 100");

        NS_TEST_ASSERT_MSG_EQ(resource->GetAvailable(),
                              100.0,
                              "Availability should be 100");

        NS_TEST_ASSERT_MSG_EQ(resource->GetUtilization(),
                              0.0,
                              "Initial utilization should be zero");

        // Consume resource
        NS_TEST_ASSERT_MSG_EQ(resource->Consume(30.0),
                              true,
                              "Valid consumption should succeed");

        NS_TEST_ASSERT_MSG_EQ(resource->GetAvailable(),
                              70.0,
                              "Availability should decrease after consumption");

        NS_TEST_ASSERT_MSG_EQ_TOL(resource->GetUtilization(),
                                   0.30,
                                   1e-12,
                                   "Utilization should be 30%");

        // Prevent over-consumption
        NS_TEST_ASSERT_MSG_EQ(resource->Consume(80.0),
                              false,
                              "Over-consumption should fail");

        NS_TEST_ASSERT_MSG_EQ(resource->GetAvailable(),
                              70.0,
                              "Failed consumption must not change availability");

        // Release resource
        NS_TEST_ASSERT_MSG_EQ(resource->Release(20.0),
                              true,
                              "Valid release should succeed");

        NS_TEST_ASSERT_MSG_EQ(resource->GetAvailable(),
                              90.0,
                              "Availability should increase after release");

        // Prevent releasing beyond capacity
        NS_TEST_ASSERT_MSG_EQ(resource->Release(20.0),
                              false,
                              "Release beyond capacity should fail");

        NS_TEST_ASSERT_MSG_EQ(resource->GetAvailable(),
                              90.0,
                              "Failed release must not change availability");

        // Reject invalid availability
        NS_TEST_ASSERT_MSG_EQ(resource->SetAvailable(110.0),
                              false,
                              "Availability above capacity should fail");

        NS_TEST_ASSERT_MSG_EQ(resource->GetAvailable(),
                              90.0,
                              "Invalid availability must not change state");

        // Reject invalid capacity that would discard allocated resource
        NS_TEST_ASSERT_MSG_EQ(resource->SetCapacity(5.0),
                              false,
                              "Capacity below allocated amount should fail");

        NS_TEST_ASSERT_MSG_EQ(resource->GetCapacity(),
                              100.0,
                              "Invalid capacity must not change state");

        // Reject invalid numeric values
        NS_TEST_ASSERT_MSG_EQ(resource->SetCapacity(-1.0),
                              false,
                              "Negative capacity should fail");

        NS_TEST_ASSERT_MSG_EQ(resource->SetAvailable(-1.0),
                              false,
                              "Negative availability should fail");

        NS_TEST_ASSERT_MSG_EQ(resource->Consume(-1.0),
                              false,
                              "Negative consumption should fail");

        NS_TEST_ASSERT_MSG_EQ(resource->Release(-1.0),
                              false,
                              "Negative release should fail");

        NS_TEST_ASSERT_MSG_EQ(resource->Consume(std::nan("")),
                              false,
                              "NaN consumption should fail");
    }
};

class SixGResourceTestSuite : public TestSuite
{
  public:
    SixGResourceTestSuite()
        : TestSuite("6g-resource", Type::UNIT)
    {
        AddTestCase(new SixGResourceTestCase, TestCase::Duration::QUICK);
    }
};

static SixGResourceTestSuite g_sixGResourceTestSuite;
