// Include a header file from your module to test.
#include "ns3/network-nodes.h"
#include "ns3/6g-ue.h"
#include "ns3/6g-access-node.h"
#include "ns3/6g-edge-node.h"
#include "ns3/6g-service.h"
#include "ns3/6g-service-session.h"

// An essential include is test.h
#include "ns3/test.h"
#include "ns3/vector.h"

// Do not put your test classes in namespace ns3.  You may find it useful
// to use the using directive to access the ns3 namespace directly
using namespace ns3;

// Add a doxygen group for tests.
// If you have more than one test, this should be in only one of them.
/**
 * @defgroup network-nodes-tests Tests for network-nodes
 * @ingroup network-nodes
 * @ingroup tests
 */

// This is an example TestCase.
/**
 * @ingroup network-nodes-tests
 * Test case for feature 1
 */

class SixGUeTestCase : public TestCase
{
  public:
    SixGUeTestCase()
        : TestCase("Test 6G UE state and connectivity")
    {
    }

  private:
    void DoRun() override
    {
        Ptr<SixGUe> ue = CreateObject<SixGUe>();

        ue->SetNodeId("UE-1");
        ue->SetPosition(Vector(10.0, 20.0, 0.0));
        ue->SetConnectedAccessNode("AN-1");
        ue->SetConnected(true);
        ue->SetCurrentService("Service-1");

        NS_TEST_ASSERT_MSG_EQ(ue->GetNodeId(),
                              "UE-1",
                              "UE ID is incorrect");

        Vector position = ue->GetPosition();
        NS_TEST_ASSERT_MSG_EQ_TOL(position.x,
                                   10.0,
                                   1e-9,
                                   "UE X position is incorrect");

        NS_TEST_ASSERT_MSG_EQ_TOL(position.y,
                                   20.0,
                                   1e-9,
                                   "UE Y position is incorrect");

        NS_TEST_ASSERT_MSG_EQ(ue->GetConnectedAccessNode(),
                              "AN-1",
                              "Connected access node is incorrect");

        NS_TEST_ASSERT_MSG_EQ(ue->IsConnected(),
                              true,
                              "UE should be connected");

        NS_TEST_ASSERT_MSG_EQ(ue->GetCurrentService(),
                              "Service-1",
                              "Current service is incorrect");

        ue->SetConnected(false);

        NS_TEST_ASSERT_MSG_EQ(ue->IsConnected(),
                              false,
                              "UE connection state did not update");
    }
};


class SixGServiceSessionTestCase : public TestCase
{
  public:
    SixGServiceSessionTestCase()
        : TestCase("Test SixG service session")
    {
    }

  private:
    void DoRun() override
    {
        Ptr<SixGUe> ue = CreateObject<SixGUe>();
        Ptr<SixGAccessNode> accessNode = CreateObject<SixGAccessNode>();
        Ptr<SixGEdgeNode> edgeNode = CreateObject<SixGEdgeNode>();
        Ptr<SixGService> service = CreateObject<SixGService>();
        Ptr<SixGServiceSession> session =
            CreateObject<SixGServiceSession>();

        ue->SetNodeId("UE-1");
        accessNode->SetNodeId("AN-1");
        edgeNode->SetNodeId("EDGE-1");

        accessNode->SetCommunicationCapacity(100.0);
        accessNode->SetAvailableCommunication(100.0);

        edgeNode->SetComputeCapacity(80.0);
        edgeNode->SetAvailableCompute(80.0);

        service->SetServiceId("SERVICE-1");
        service->SetRequiredCommunication(30.0);
        service->SetRequiredCompute(20.0);
        service->SetActive(true);

        ue->SetActive(true);
        accessNode->SetActive(true);
        edgeNode->SetActive(true);

        session->SetSessionId("SESSION-1");
        session->SetUe(ue);
        session->SetAccessNode(accessNode);
        session->SetEdgeNode(edgeNode);
        session->SetService(service);

        NS_TEST_ASSERT_MSG_EQ(session->IsActive(),
                              false,
                              "Session should initially be inactive");

        NS_TEST_ASSERT_MSG_EQ(session->Activate(),
                              true,
                              "Session activation should succeed");

        NS_TEST_ASSERT_MSG_EQ(session->IsActive(),
                              true,
                              "Session should be active");

        NS_TEST_ASSERT_MSG_EQ_TOL(
            accessNode->GetAvailableCommunication(),
            70.0,
            1e-9,
            "Communication resource was not reserved correctly");

        NS_TEST_ASSERT_MSG_EQ_TOL(
            edgeNode->GetAvailableCompute(),
            60.0,
            1e-9,
            "Compute resource was not reserved correctly");

        NS_TEST_ASSERT_MSG_EQ(ue->IsConnected(),
                              true,
                              "UE should be connected");

        NS_TEST_ASSERT_MSG_EQ(ue->GetConnectedAccessNode(),
                              "AN-1",
                              "UE should be connected to the correct access node");

        NS_TEST_ASSERT_MSG_EQ(ue->GetCurrentService(),
                              "SERVICE-1",
                              "UE should run the correct service");

        NS_TEST_ASSERT_MSG_EQ(accessNode->GetConnectedUeCount(),
                              1u,
                              "Access node should contain one connected UE");

        NS_TEST_ASSERT_MSG_EQ(session->Deactivate(),
                              true,
                              "Session deactivation should succeed");

        NS_TEST_ASSERT_MSG_EQ(session->IsActive(),
                              false,
                              "Session should be inactive");

        NS_TEST_ASSERT_MSG_EQ_TOL(
            accessNode->GetAvailableCommunication(),
            100.0,
            1e-9,
            "Communication resource was not released correctly");

        NS_TEST_ASSERT_MSG_EQ_TOL(
            edgeNode->GetAvailableCompute(),
            80.0,
            1e-9,
            "Compute resource was not released correctly");

        NS_TEST_ASSERT_MSG_EQ(ue->IsConnected(),
                              false,
                              "UE should be disconnected");

        NS_TEST_ASSERT_MSG_EQ(ue->GetCurrentService(),
                              "",
                              "UE service should be cleared");

        NS_TEST_ASSERT_MSG_EQ(accessNode->GetConnectedUeCount(),
                              0u,
                              "Access node should have no connected UEs");
    }
};

class NetworkNodesTestCase1 : public TestCase
{
  public:
    NetworkNodesTestCase1();
    ~NetworkNodesTestCase1() override;

  private:
    void DoRun() override;
};

// Add some help text to this case to describe what it is intended to test
NetworkNodesTestCase1::NetworkNodesTestCase1()
    : TestCase("NetworkNodes test case (does nothing)")
{
}

// This destructor does nothing but we include it as a reminder that
// the test case should clean up after itself
NetworkNodesTestCase1::~NetworkNodesTestCase1()
{
}

//
// This method is the pure virtual method from class TestCase that every
// TestCase must implement
//
void
NetworkNodesTestCase1::DoRun()
{
    // A wide variety of test macros are available in src/core/test.h
    NS_TEST_ASSERT_MSG_EQ(true, true, "true doesn't equal true for some reason");
    // Use this one for floating point comparisons
    NS_TEST_ASSERT_MSG_EQ_TOL(0.01, 0.01, 0.001, "Numbers are not equal within tolerance");
}

// The TestSuite class names the TestSuite, identifies what type of TestSuite,
// and enables the TestCases to be run.  Typically, only the constructor for
// this class must be defined

/**
 * @ingroup network-nodes-tests
 * TestSuite for module network-nodes
 */

class SixGAccessNodeTestCase : public TestCase
{
  public:
    SixGAccessNodeTestCase()
        : TestCase("Test 6G Access Node state and connected UEs")
    {
    }

  private:
    void DoRun() override
    {
        Ptr<SixGAccessNode> accessNode = CreateObject<SixGAccessNode>();

        accessNode->SetNodeId("AN-1");
        accessNode->SetPosition(Vector(100.0, 200.0, 10.0));
        accessNode->SetCommunicationCapacity(1000.0);
        accessNode->SetAvailableCommunication(750.0);
        accessNode->SetCoverageRadius(500.0);
        accessNode->SetLoad(0.25);

        accessNode->AddConnectedUe("UE-1");
        accessNode->AddConnectedUe("UE-2");
        accessNode->AddConnectedUe("UE-1");

        NS_TEST_ASSERT_MSG_EQ(accessNode->GetNodeId(),
                              "AN-1",
                              "Access Node ID is incorrect");

        Vector position = accessNode->GetPosition();

        NS_TEST_ASSERT_MSG_EQ_TOL(position.x,
                                   100.0,
                                   1e-9,
                                   "Access Node X position is incorrect");

        NS_TEST_ASSERT_MSG_EQ_TOL(position.y,
                                   200.0,
                                   1e-9,
                                   "Access Node Y position is incorrect");

        NS_TEST_ASSERT_MSG_EQ_TOL(position.z,
                                   10.0,
                                   1e-9,
                                   "Access Node Z position is incorrect");

        NS_TEST_ASSERT_MSG_EQ_TOL(accessNode->GetCommunicationCapacity(),
                                   1000.0,
                                   1e-9,
                                   "Communication capacity is incorrect");

        NS_TEST_ASSERT_MSG_EQ_TOL(accessNode->GetAvailableCommunication(),
                                   750.0,
                                   1e-9,
                                   "Available communication is incorrect");

        NS_TEST_ASSERT_MSG_EQ_TOL(accessNode->GetCoverageRadius(),
                                   500.0,
                                   1e-9,
                                   "Coverage radius is incorrect");

        NS_TEST_ASSERT_MSG_EQ_TOL(accessNode->GetLoad(),
                                   0.25,
                                   1e-9,
                                   "Load is incorrect");

        NS_TEST_ASSERT_MSG_EQ(accessNode->GetConnectedUeCount(),
                              2u,
                              "Duplicate UE should not be added");

        NS_TEST_ASSERT_MSG_EQ(accessNode->HasConnectedUe("UE-1"),
                              true,
                              "UE-1 should be connected");

        NS_TEST_ASSERT_MSG_EQ(accessNode->HasConnectedUe("UE-3"),
                              false,
                              "UE-3 should not be connected");

        accessNode->RemoveConnectedUe("UE-1");

        NS_TEST_ASSERT_MSG_EQ(accessNode->GetConnectedUeCount(),
                              1u,
                              "UE was not removed");

        NS_TEST_ASSERT_MSG_EQ(accessNode->HasConnectedUe("UE-1"),
                              false,
                              "UE-1 should no longer be connected");
    }
};


class SixGEdgeNodeTestCase : public TestCase
{
  public:
    SixGEdgeNodeTestCase()
        : TestCase("Test 6G Edge Node computing state")
    {
    }

  private:
    void DoRun() override
    {
        Ptr<SixGEdgeNode> edgeNode = CreateObject<SixGEdgeNode>();

        NS_TEST_ASSERT_MSG_EQ(edgeNode->GetConnectedCoreId(),
                              std::string(""),
                              "Edge Node should initially have no Core");

        edgeNode->SetConnectedCoreId("CoreNode-1");

        NS_TEST_ASSERT_MSG_EQ(edgeNode->GetConnectedCoreId(),
                              std::string("CoreNode-1"),
                              "Edge Node Core association is incorrect");

        edgeNode->SetConnectedCoreId("CoreNode-2");

        NS_TEST_ASSERT_MSG_EQ(edgeNode->GetConnectedCoreId(),
                              std::string("CoreNode-2"),
                              "Edge Node Core reassociation failed");

        edgeNode->SetConnectedCoreId("");

        NS_TEST_ASSERT_MSG_EQ(edgeNode->GetConnectedCoreId(),
                              std::string(""),
                              "Edge Node Core disconnection failed");

        edgeNode->SetNodeId("EDGE-1");
        edgeNode->SetPosition(Vector(50.0, 75.0, 5.0));

        edgeNode->SetComputeCapacity(2000.0);
        edgeNode->SetAvailableCompute(1500.0);

        edgeNode->SetMemoryCapacity(64.0);
        edgeNode->SetAvailableMemory(48.0);

        edgeNode->SetApplicationLoad(0.25);

        NS_TEST_ASSERT_MSG_EQ(edgeNode->GetNodeId(),
                              "EDGE-1",
                              "Edge Node ID is incorrect");

        Vector position = edgeNode->GetPosition();

        NS_TEST_ASSERT_MSG_EQ_TOL(position.x,
                                   50.0,
                                   1e-9,
                                   "Edge Node X position is incorrect");

        NS_TEST_ASSERT_MSG_EQ_TOL(position.y,
                                   75.0,
                                   1e-9,
                                   "Edge Node Y position is incorrect");

        NS_TEST_ASSERT_MSG_EQ_TOL(position.z,
                                   5.0,
                                   1e-9,
                                   "Edge Node Z position is incorrect");

        NS_TEST_ASSERT_MSG_EQ_TOL(edgeNode->GetComputeCapacity(),
                                   2000.0,
                                   1e-9,
                                   "Compute capacity is incorrect");

        NS_TEST_ASSERT_MSG_EQ_TOL(edgeNode->GetAvailableCompute(),
                                   1500.0,
                                   1e-9,
                                   "Available compute is incorrect");

        NS_TEST_ASSERT_MSG_EQ_TOL(edgeNode->GetMemoryCapacity(),
                                   64.0,
                                   1e-9,
                                   "Memory capacity is incorrect");

        NS_TEST_ASSERT_MSG_EQ_TOL(edgeNode->GetAvailableMemory(),
                                   48.0,
                                   1e-9,
                                   "Available memory is incorrect");

        NS_TEST_ASSERT_MSG_EQ_TOL(edgeNode->GetApplicationLoad(),
                                   0.25,
                                   1e-9,
                                   "Application load is incorrect");

        edgeNode->SetAvailableCompute(500.0);
        edgeNode->SetAvailableMemory(16.0);
        edgeNode->SetApplicationLoad(0.75);

        NS_TEST_ASSERT_MSG_EQ_TOL(edgeNode->GetAvailableCompute(),
                                   500.0,
                                   1e-9,
                                   "Available compute did not update");

        NS_TEST_ASSERT_MSG_EQ_TOL(edgeNode->GetAvailableMemory(),
                                   16.0,
                                   1e-9,
                                   "Available memory did not update");

        NS_TEST_ASSERT_MSG_EQ_TOL(edgeNode->GetApplicationLoad(),
                                   0.75,
                                   1e-9,
                                   "Application load did not update");
    }
};


class SixGServiceTestCase : public TestCase
{
  public:
    SixGServiceTestCase()
        : TestCase("Test 6G service requirements and state")
    {
    }

  private:
    void DoRun() override
    {
        Ptr<SixGService> service = CreateObject<SixGService>();

        service->SetServiceId("AI-Service-1");
        service->SetRequiredCompute(500.0);
        service->SetRequiredCommunication(100.0);
        service->SetMaximumLatency(5.0);
        service->SetMinimumReliability(0.999);
        service->SetActive(true);

        NS_TEST_ASSERT_MSG_EQ(service->GetServiceId(),
                              "AI-Service-1",
                              "Service ID is incorrect");

        NS_TEST_ASSERT_MSG_EQ_TOL(service->GetRequiredCompute(),
                                   500.0,
                                   1e-9,
                                   "Required compute is incorrect");

        NS_TEST_ASSERT_MSG_EQ_TOL(service->GetRequiredCommunication(),
                                   100.0,
                                   1e-9,
                                   "Required communication is incorrect");

        NS_TEST_ASSERT_MSG_EQ_TOL(service->GetMaximumLatency(),
                                   5.0,
                                   1e-9,
                                   "Maximum latency is incorrect");

        NS_TEST_ASSERT_MSG_EQ_TOL(service->GetMinimumReliability(),
                                   0.999,
                                   1e-12,
                                   "Minimum reliability is incorrect");

        NS_TEST_ASSERT_MSG_EQ(service->IsActive(),
                              true,
                              "Service should be active");

        service->SetActive(false);

        NS_TEST_ASSERT_MSG_EQ(service->IsActive(),
                              false,
                              "Service active state did not update");
    }
};

class NetworkNodesTestSuite : public TestSuite
{
  public:
    NetworkNodesTestSuite();
};

// Type for TestSuite can be UNIT, SYSTEM, EXAMPLE, or PERFORMANCE
NetworkNodesTestSuite::NetworkNodesTestSuite()
    : TestSuite("network-nodes", Type::UNIT)
{
    // Duration for TestCase can be QUICK, EXTENSIVE or TAKES_FOREVER
    AddTestCase(new NetworkNodesTestCase1, TestCase::Duration::QUICK);
    AddTestCase(new SixGUeTestCase, TestCase::Duration::QUICK);
    AddTestCase(new SixGAccessNodeTestCase, TestCase::Duration::QUICK);
    AddTestCase(new SixGEdgeNodeTestCase, TestCase::Duration::QUICK);
    AddTestCase(new SixGServiceTestCase, TestCase::Duration::QUICK);
}

// Do not forget to allocate an instance of this TestSuite
/**
 * @ingroup network-nodes-tests
 * Static variable for test initialization
 */
static NetworkNodesTestSuite snetworkNodesTestSuite;
