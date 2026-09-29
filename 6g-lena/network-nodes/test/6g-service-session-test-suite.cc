#include "ns3/6g-access-node.h"
#include "ns3/6g-capability-provider.h"
#include "ns3/6g-core.h"
#include "ns3/6g-edge-node.h"
#include "ns3/6g-service.h"
#include "ns3/6g-service-session.h"
#include "ns3/6g-ue.h"
#include "ns3/test.h"

using namespace ns3;

class SixGServiceSessionTestCase : public TestCase
{
  public:
    SixGServiceSessionTestCase()
        : TestCase("Test SixG service session activation and resource lifecycle")
    {
    }

  private:
    void DoRun() override
    {
        Ptr<SixGCore> core = CreateObject<SixGCore>();

        Ptr<SixGAccessNode> access = CreateObject<SixGAccessNode>();
        access->SetNodeId("AccessNode-Test");
        access->SetActive(true);
        access->SetCommunicationCapacity(100.0);
        access->SetAvailableCommunication(100.0);

        Ptr<SixGEdgeNode> edge = CreateObject<SixGEdgeNode>();
        edge->SetNodeId("EdgeNode-Test");
        edge->SetActive(true);
        edge->SetComputeCapacity(100.0);
        edge->SetAvailableCompute(100.0);

        Ptr<SixGUe> ue = CreateObject<SixGUe>();
        ue->SetNodeId("UE-Test");
        ue->SetActive(true);

        access->AddReachableEdgeNode(edge);

        Ptr<SixGService> service = CreateObject<SixGService>();
        service->SetServiceId("Service-Test");
        service->SetRequiredCompute(60.0);
        service->SetRequiredCommunication(20.0);
        service->SetActive(true);

        Ptr<SixGCapabilityProvider> provider =
            CreateObject<SixGCapabilityProvider>();

        provider->SetCore(core);
        provider->SetNodeId("EdgeNode-Test");
        provider->SetCapabilityType("COMPUTING");
        provider->SetResource(edge->GetResource("COMPUTING"));
        provider->SetAvailable(true);
        provider->SetValidityDuration(Seconds(10.0));

        NS_TEST_ASSERT_MSG_EQ(provider->Register(),
                              true,
                              "Capability provider registration failed");

        Ptr<SixGServiceSession> session =
            CreateObject<SixGServiceSession>();

        session->SetSessionId("Session-Test");
        session->SetUe(ue);
        session->SetAccessNode(access);
        session->SetEdgeNode(edge);
        session->SetService(service);
        session->SetCapabilityProvider(provider);

        NS_TEST_ASSERT_MSG_EQ(session->Activate(),
                              true,
                              "Valid service session should activate");

        NS_TEST_ASSERT_MSG_EQ(session->IsActive(),
                              true,
                              "Session should be active after activation");

        NS_TEST_ASSERT_MSG_EQ_TOL(edge->GetAvailableCompute(),
                                  40.0,
                                  1e-9,
                                  "Edge compute reservation incorrect");

        NS_TEST_ASSERT_MSG_EQ_TOL(access->GetAvailableCommunication(),
                                  80.0,
                                  1e-9,
                                  "Communication reservation incorrect");

        NS_TEST_ASSERT_MSG_EQ(ue->IsConnected(),
                              true,
                              "UE should be connected after activation");

        NS_TEST_ASSERT_MSG_EQ(ue->GetCurrentService(),
                              std::string("Service-Test"),
                              "UE service association incorrect");

        NS_TEST_ASSERT_MSG_EQ(session->Deactivate(),
                              true,
                              "Active service session should deactivate");

        NS_TEST_ASSERT_MSG_EQ(session->IsActive(),
                              false,
                              "Session should be inactive after deactivation");

        NS_TEST_ASSERT_MSG_EQ_TOL(edge->GetAvailableCompute(),
                                  100.0,
                                  1e-9,
                                  "Edge compute was not restored");

        NS_TEST_ASSERT_MSG_EQ_TOL(access->GetAvailableCommunication(),
                                  100.0,
                                  1e-9,
                                  "Communication resource was not restored");

        NS_TEST_ASSERT_MSG_EQ(ue->IsConnected(),
                              false,
                              "UE should be disconnected after deactivation");

        NS_TEST_ASSERT_MSG_EQ(ue->GetCurrentService(),
                              std::string(""),
                              "UE service association was not cleared");

        Ptr<SixGService> oversizedService =
            CreateObject<SixGService>();

        oversizedService->SetServiceId("Oversized-Service");
        oversizedService->SetRequiredCompute(120.0);
        oversizedService->SetRequiredCommunication(20.0);
        oversizedService->SetActive(true);

        Ptr<SixGServiceSession> failedSession =
            CreateObject<SixGServiceSession>();

        failedSession->SetSessionId("Failed-Session");
        failedSession->SetUe(ue);
        failedSession->SetAccessNode(access);
        failedSession->SetEdgeNode(edge);
        failedSession->SetService(oversizedService);
        failedSession->SetCapabilityProvider(provider);

        NS_TEST_ASSERT_MSG_EQ(failedSession->Activate(),
                              false,
                              "Oversized service should not activate");

        NS_TEST_ASSERT_MSG_EQ(failedSession->IsActive(),
                              false,
                              "Failed session must remain inactive");

        NS_TEST_ASSERT_MSG_EQ_TOL(edge->GetAvailableCompute(),
                                  100.0,
                                  1e-9,
                                  "Failed activation changed compute state");

        NS_TEST_ASSERT_MSG_EQ_TOL(access->GetAvailableCommunication(),
                                  100.0,
                                  1e-9,
                                  "Failed activation changed communication state");
    }
};

class SixGServiceSessionTestSuite : public TestSuite
{
  public:
    SixGServiceSessionTestSuite()
        : TestSuite("6g-service-session", Type::UNIT)
    {
        AddTestCase(new SixGServiceSessionTestCase,
                    TestCase::Duration::QUICK);
    }
};

static SixGServiceSessionTestSuite g_sixGServiceSessionTestSuite;
