#include "ns3/6g-capability-matcher.h"
#include "ns3/6g-capability-provider.h"
#include "ns3/6g-compute-task.h"
#include "ns3/6g-core.h"
#include "ns3/test.h"
#include "ns3/simulator.h"
#include "ns3/constant-velocity-mobility-model.h"
#include "ns3/nstime.h"

#include <vector>

namespace ns3
{

class SixGCoreTestCase1 : public TestCase
{
  public:
    SixGCoreTestCase1()
        : TestCase("Test 6G core capability registration and discovery")
    {
    }

  private:
    void DoRun() override
    {
        Ptr<SixGCore> core = CreateObject<SixGCore>();

        core->SetScenario("Test-Scenario");
        core->Initialize();

        NS_TEST_ASSERT_MSG_EQ(core->IsInitialized(),
                              true,
                              "Core should be initialized");

        SixGCapability capability;
        capability.nodeId = "Node-A";
        capability.type = "COMPUTING";
        capability.capacity = 100.0;
        capability.availableResource = 100.0;
        capability.available = true;

        NS_TEST_ASSERT_MSG_EQ(core->RegisterCapability(capability),
                              true,
                              "Capability registration should succeed");

        NS_TEST_ASSERT_MSG_EQ(core->HasCapability("Node-A", "COMPUTING"),
                              true,
                              "Capability should exist");

        SixGCapabilityRequest request;
        request.type = "COMPUTING";
        request.minimumResource = 50.0;
        request.requireAvailable = true;

        auto matches = core->DiscoverCapabilities(request);

        NS_TEST_ASSERT_MSG_EQ(matches.size(),
                              1u,
                              "One capability should match");

        NS_TEST_ASSERT_MSG_EQ_TOL(matches[0].capacity,
                                  100.0,
                                  1e-9,
                                  "Capacity should be 100");

        NS_TEST_ASSERT_MSG_EQ_TOL(matches[0].availableResource,
                                  100.0,
                                  1e-9,
                                  "Available resource should be 100");

        capability.availableResource = 40.0;

        NS_TEST_ASSERT_MSG_EQ(core->UpdateCapability(capability),
                              true,
                              "Capability update should succeed");

        matches = core->DiscoverCapabilities(request);

        NS_TEST_ASSERT_MSG_EQ(matches.size(),
                              0u,
                              "Capability with only 40 available resource should not match");

        capability.availableResource = 80.0;
        capability.available = false;

        NS_TEST_ASSERT_MSG_EQ(core->UpdateCapability(capability),
                              true,
                              "Capability availability update should succeed");

        matches = core->DiscoverCapabilities(request);

        NS_TEST_ASSERT_MSG_EQ(matches.size(),
                              0u,
                              "Unavailable capability should not match");

        NS_TEST_ASSERT_MSG_EQ(core->RemoveCapability("Node-A", "COMPUTING"),
                              true,
                              "Capability removal should succeed");

        NS_TEST_ASSERT_MSG_EQ(core->HasCapability("Node-A", "COMPUTING"),
                              false,
                              "Capability should be removed");
    }
};


class SixGLifecycleDiscoveryTestCase : public TestCase
{
  public:
    SixGLifecycleDiscoveryTestCase()
        : TestCase("Tests lifecycle state effects on capability discovery")
    {
    }

  private:
    void DoRun() override
    {
        Ptr<SixGCore> core = CreateObject<SixGCore>();
        core->Initialize();

        Ptr<SixGCapabilityProvider> provider =
            CreateObject<SixGCapabilityProvider>();

        provider->SetCore(core);
        provider->SetNodeId("Node-Lifecycle");
        provider->SetCapabilityType("COMPUTING");
        provider->SetCapacity(100.0);

        NS_TEST_ASSERT_MSG_EQ(provider->Register(),
                              true,
                              "Capability registration should succeed");

        SixGCapabilityRequest request;
        request.type = "COMPUTING";
        request.minimumResource = 20.0;
        request.requireAvailable = true;

        auto matches = core->DiscoverCapabilities(request);

        NS_TEST_ASSERT_MSG_EQ(matches.size(),
                              1u,
                              "ACTIVE capability should be discoverable");

        NS_TEST_ASSERT_MSG_EQ(static_cast<int>(matches[0].state),
                              static_cast<int>(SixGCapabilityState::ACTIVE),
                              "Initial state should be ACTIVE");

        provider->SetState(SixGCapabilityState::DEGRADED);

        matches = core->DiscoverCapabilities(request);

        NS_TEST_ASSERT_MSG_EQ(matches.size(),
                              1u,
                              "DEGRADED capability should remain discoverable");

        NS_TEST_ASSERT_MSG_EQ(static_cast<int>(matches[0].state),
                              static_cast<int>(SixGCapabilityState::DEGRADED),
                              "Capability should be DEGRADED");

        provider->SetState(SixGCapabilityState::UNAVAILABLE);

        matches = core->DiscoverCapabilities(request);

        NS_TEST_ASSERT_MSG_EQ(matches.size(),
                              0u,
                              "UNAVAILABLE capability should not be discoverable");
    }
};

class SixGCapabilityStateTestCase : public TestCase
{
  public:
    SixGCapabilityStateTestCase()
        : TestCase("Tests capability lifecycle state transitions")
    {
    }

  private:
    void DoRun() override
    {
        Ptr<SixGCore> core = CreateObject<SixGCore>();
        core->Initialize();

        Ptr<SixGCapabilityProvider> provider =
            CreateObject<SixGCapabilityProvider>();

        provider->SetCore(core);
        provider->SetNodeId("Node-State");
        provider->SetCapabilityType("COMPUTING");
        provider->SetCapacity(100.0);

        NS_TEST_ASSERT_MSG_EQ(static_cast<int>(provider->GetState()),
                              static_cast<int>(SixGCapabilityState::ACTIVE),
                              "Initial capability state should be ACTIVE");

        NS_TEST_ASSERT_MSG_EQ(provider->Register(),
                              true,
                              "Capability registration should succeed");

        auto capabilities = core->GetCapabilities();

        NS_TEST_ASSERT_MSG_EQ(capabilities.size(),
                              1u,
                              "Exactly one capability should be registered");

        NS_TEST_ASSERT_MSG_EQ(static_cast<int>(capabilities[0].state),
                              static_cast<int>(SixGCapabilityState::ACTIVE),
                              "Registered capability should initially be ACTIVE");

        provider->SetState(SixGCapabilityState::DEGRADED);

        NS_TEST_ASSERT_MSG_EQ(static_cast<int>(provider->GetState()),
                              static_cast<int>(SixGCapabilityState::DEGRADED),
                              "Provider state should become DEGRADED");

        capabilities = core->GetCapabilities();

        NS_TEST_ASSERT_MSG_EQ(static_cast<int>(capabilities[0].state),
                              static_cast<int>(SixGCapabilityState::DEGRADED),
                              "Core registry should receive DEGRADED state");

        provider->SetState(SixGCapabilityState::UNAVAILABLE);

        NS_TEST_ASSERT_MSG_EQ(static_cast<int>(provider->GetState()),
                              static_cast<int>(SixGCapabilityState::UNAVAILABLE),
                              "Provider state should become UNAVAILABLE");

        capabilities = core->GetCapabilities();

        NS_TEST_ASSERT_MSG_EQ(static_cast<int>(capabilities[0].state),
                              static_cast<int>(SixGCapabilityState::UNAVAILABLE),
                              "Core registry should receive UNAVAILABLE state");
    }
};

class SixGCapabilityTemporalTestCase : public TestCase
{
  public:
    SixGCapabilityTemporalTestCase()
        : TestCase("Tests time-aware capability registration and updates")
    {
    }

  private:
    void DoRun() override
    {
        Ptr<SixGCore> core = CreateObject<SixGCore>();
        core->Initialize();

        SixGCapability capability;
        capability.nodeId = "Node-Time";
        capability.type = "COMPUTING";
        capability.capacity = 100.0;
        capability.availableResource = 100.0;
        capability.available = true;

        NS_TEST_ASSERT_MSG_EQ(core->RegisterCapability(capability),
                              true,
                              "Capability registration should succeed");

        auto capabilities = core->GetCapabilities();

        NS_TEST_ASSERT_MSG_EQ(capabilities.size(),
                              1u,
                              "Exactly one capability should be registered");

        NS_TEST_ASSERT_MSG_EQ_TOL(capabilities[0].lastUpdated.GetSeconds(),
                                  0.0,
                                  1e-9,
                                  "Initial registration should occur at t=0");

        Simulator::Schedule(Seconds(5.0),
                            [core]()
                            {
                                SixGCapability updated;
                                updated.nodeId = "Node-Time";
                                updated.type = "COMPUTING";
                                updated.capacity = 100.0;
                                updated.availableResource = 40.0;
                                updated.available = true;

                                NS_ABORT_MSG_UNLESS(core->UpdateCapability(updated),
                                                    "Capability update should succeed");
                            });

        Simulator::Run();

        capabilities = core->GetCapabilities();

        NS_TEST_ASSERT_MSG_EQ_TOL(capabilities[0].lastUpdated.GetSeconds(),
                                  5.0,
                                  1e-9,
                                  "Capability update should refresh lastUpdated to simulation time");

        NS_TEST_ASSERT_MSG_EQ_TOL(capabilities[0].availableResource,
                                  40.0,
                                  1e-9,
                                  "Updated resource value should be stored");

        Simulator::Destroy();
    }
};

class SixGCapabilityFreshnessTestCase : public TestCase
{
  public:
    SixGCapabilityFreshnessTestCase()
        : TestCase("Tests capability freshness using simulation time")
    {
    }

  private:
    void DoRun() override
    {
        Ptr<SixGCore> core = CreateObject<SixGCore>();
        core->Initialize();

        SixGCapability capability;
        capability.nodeId = "Node-Fresh";
        capability.type = "COMPUTING";
        capability.capacity = 100.0;
        capability.availableResource = 100.0;
        capability.available = true;
        capability.validityDuration = Seconds(10.0);

        NS_TEST_ASSERT_MSG_EQ(core->RegisterCapability(capability),
                              true,
                              "Capability registration should succeed");

        auto capabilities = core->GetCapabilities();

        NS_TEST_ASSERT_MSG_EQ(core->IsCapabilityFresh(capabilities[0]),
                              true,
                              "Capability should be fresh immediately after registration");

        Simulator::Schedule(Seconds(5.0),
                            [core]()
                            {
                                auto capabilities = core->GetCapabilities();

                                NS_ABORT_MSG_UNLESS(
                                    core->IsCapabilityFresh(capabilities[0]),
                                    "Capability should still be fresh at t=5s");
                            });

        Simulator::Schedule(Seconds(11.0),
                            [core]()
                            {
                                auto capabilities = core->GetCapabilities();

                                NS_ABORT_MSG_UNLESS(
                                    !core->IsCapabilityFresh(capabilities[0]),
                                    "Capability should be stale after validity duration expires");
                            });

        Simulator::Run();
        Simulator::Destroy();
    }
};

class SixGFreshDiscoveryTestCase : public TestCase
{
  public:
    SixGFreshDiscoveryTestCase()
        : TestCase("Tests freshness-aware capability discovery")
    {
    }

  private:
    void DoRun() override
    {
        Ptr<SixGCore> core = CreateObject<SixGCore>();
        core->Initialize();

        SixGCapability capability;
        capability.nodeId = "Node-Fresh-Discovery";
        capability.type = "COMPUTING";
        capability.capacity = 100.0;
        capability.availableResource = 80.0;
        capability.available = true;
        capability.validityDuration = Seconds(5.0);

        NS_TEST_ASSERT_MSG_EQ(core->RegisterCapability(capability),
                              true,
                              "Capability registration should succeed");

        SixGCapabilityRequest request;
        request.type = "COMPUTING";
        request.minimumResource = 20.0;
        request.requireAvailable = true;
        request.requireFresh = true;

        auto matches = core->DiscoverCapabilities(request);

        NS_TEST_ASSERT_MSG_EQ(matches.size(),
                              1u,
                              "Fresh capability should be discovered");

        Simulator::Schedule(Seconds(6.0),
                            [core]()
                            {
                                SixGCapabilityRequest freshRequest;
                                freshRequest.type = "COMPUTING";
                                freshRequest.minimumResource = 20.0;
                                freshRequest.requireAvailable = true;
                                freshRequest.requireFresh = true;

                                auto freshMatches =
                                    core->DiscoverCapabilities(freshRequest);

                                NS_ABORT_MSG_UNLESS(
                                    freshMatches.empty(),
                                    "Stale capability should not be discovered when requireFresh is true");

                                SixGCapabilityRequest normalRequest;
                                normalRequest.type = "COMPUTING";
                                normalRequest.minimumResource = 20.0;
                                normalRequest.requireAvailable = true;
                                normalRequest.requireFresh = false;

                                auto normalMatches =
                                    core->DiscoverCapabilities(normalRequest);

                                NS_ABORT_MSG_UNLESS(
                                    normalMatches.size() == 1u,
                                    "Stale capability should still be discoverable when requireFresh is false");
                            });

        Simulator::Run();
        Simulator::Destroy();
    }
};

class SixGAttributeDiscoveryTestCase : public TestCase
{
  public:
    SixGAttributeDiscoveryTestCase()
        : TestCase("Test attribute-aware heterogeneous capability discovery")
    {
    }

  private:
    void DoRun() override
    {
        Ptr<SixGCore> core = CreateObject<SixGCore>();

        SixGCapability cpuNode;
        cpuNode.nodeId = "Node-A";
        cpuNode.type = "COMPUTING";
        cpuNode.capacity = 100.0;
        cpuNode.availableResource = 100.0;
        cpuNode.available = true;
        cpuNode.attributes["accelerator"] = "CPU";

        SixGCapability gpuNode;
        gpuNode.nodeId = "Node-B";
        gpuNode.type = "COMPUTING";
        gpuNode.capacity = 80.0;
        gpuNode.availableResource = 80.0;
        gpuNode.available = true;
        gpuNode.attributes["accelerator"] = "GPU";

        SixGCapability lowGpuNode;
        lowGpuNode.nodeId = "Node-C";
        lowGpuNode.type = "COMPUTING";
        lowGpuNode.capacity = 30.0;
        lowGpuNode.availableResource = 30.0;
        lowGpuNode.available = true;
        lowGpuNode.attributes["accelerator"] = "GPU";

        NS_TEST_ASSERT_MSG_EQ(core->RegisterCapability(cpuNode),
                              true,
                              "CPU capability registration should succeed");

        NS_TEST_ASSERT_MSG_EQ(core->RegisterCapability(gpuNode),
                              true,
                              "GPU capability registration should succeed");

        NS_TEST_ASSERT_MSG_EQ(core->RegisterCapability(lowGpuNode),
                              true,
                              "Low-resource GPU registration should succeed");

        SixGCapabilityRequest request;
        request.type = "COMPUTING";
        request.minimumResource = 50.0;
        request.requireAvailable = true;
        request.requiredAttributes["accelerator"] = "GPU";

        auto matches = core->DiscoverCapabilities(request);

        NS_TEST_ASSERT_MSG_EQ(matches.size(),
                              1u,
                              "Only one capability should satisfy all constraints");

        NS_TEST_ASSERT_MSG_EQ(matches[0].nodeId,
                              "Node-B",
                              "Node-B should be the matching GPU capability");

        NS_TEST_ASSERT_MSG_EQ(matches[0].attributes.at("accelerator"),
                              "GPU",
                              "Matching capability should have GPU accelerator");

        NS_TEST_ASSERT_MSG_EQ_TOL(matches[0].availableResource,
                                  80.0,
                                  1e-9,
                                  "Matching GPU should have 80 available resources");
    }
};


class SixGConstraintDiscoveryTestCase : public TestCase
{
  public:
    SixGConstraintDiscoveryTestCase()
        : TestCase("Test constraint-based heterogeneous capability discovery")
    {
    }

  private:
    void DoRun() override
    {
        Ptr<SixGCore> core = CreateObject<SixGCore>();

        SixGCapability nodeA;
        nodeA.nodeId = "Node-A";
        nodeA.type = "COMPUTING";
        nodeA.capacity = 100.0;
        nodeA.availableResource = 100.0;
        nodeA.available = true;
        nodeA.attributes["accelerator"] = "GPU";
        nodeA.attributes["memory"] = "8";
        nodeA.attributes["latency"] = "5";

        SixGCapability nodeB;
        nodeB.nodeId = "Node-B";
        nodeB.type = "COMPUTING";
        nodeB.capacity = 100.0;
        nodeB.availableResource = 100.0;
        nodeB.available = true;
        nodeB.attributes["accelerator"] = "GPU";
        nodeB.attributes["memory"] = "32";
        nodeB.attributes["latency"] = "8";

        SixGCapability nodeC;
        nodeC.nodeId = "Node-C";
        nodeC.type = "COMPUTING";
        nodeC.capacity = 100.0;
        nodeC.availableResource = 100.0;
        nodeC.available = true;
        nodeC.attributes["accelerator"] = "CPU";
        nodeC.attributes["memory"] = "32";
        nodeC.attributes["latency"] = "5";

        NS_TEST_ASSERT_MSG_EQ(core->RegisterCapability(nodeA),
                              true,
                              "Node-A registration should succeed");

        NS_TEST_ASSERT_MSG_EQ(core->RegisterCapability(nodeB),
                              true,
                              "Node-B registration should succeed");

        NS_TEST_ASSERT_MSG_EQ(core->RegisterCapability(nodeC),
                              true,
                              "Node-C registration should succeed");

        SixGCapabilityRequest request;
        request.type = "COMPUTING";
        request.minimumResource = 50.0;
        request.requireAvailable = true;

        SixGCapabilityConstraint accelerator;
        accelerator.attribute = "accelerator";
        accelerator.op = SixGConstraintOperator::EQUAL;
        accelerator.value = "GPU";

        SixGCapabilityConstraint memory;
        memory.attribute = "memory";
        memory.op = SixGConstraintOperator::GREATER_EQUAL;
        memory.value = "16";

        SixGCapabilityConstraint latency;
        latency.attribute = "latency";
        latency.op = SixGConstraintOperator::LESS_EQUAL;
        latency.value = "10";

        request.constraints.push_back(accelerator);
        request.constraints.push_back(memory);
        request.constraints.push_back(latency);

        auto matches = core->DiscoverCapabilities(request);

        NS_TEST_ASSERT_MSG_EQ(matches.size(),
                              1u,
                              "Exactly one capability should satisfy all constraints");

        NS_TEST_ASSERT_MSG_EQ(matches[0].nodeId,
                              "Node-B",
                              "Node-B should satisfy all constraints");

        NS_TEST_ASSERT_MSG_EQ(matches[0].attributes.at("accelerator"),
                              "GPU",
                              "Selected capability should provide GPU acceleration");

        NS_TEST_ASSERT_MSG_EQ(matches[0].attributes.at("memory"),
                              "32",
                              "Selected capability should provide at least 16 GB memory");

        NS_TEST_ASSERT_MSG_EQ(matches[0].attributes.at("latency"),
                              "8",
                              "Selected capability should satisfy latency constraint");
    }
};

class SixGConstraintEdgeCaseTestCase : public TestCase
{
  public:
    SixGConstraintEdgeCaseTestCase()
        : TestCase("Test constraint edge cases")
    {
    }

  private:
    void DoRun() override
    {
        Ptr<SixGCore> core = CreateObject<SixGCore>();

        SixGCapability node;
        node.nodeId = "Node-Edge";
        node.type = "COMPUTING";
        node.capacity = 100.0;
        node.availableResource = 100.0;
        node.available = true;

        node.attributes["accelerator"] = "GPU";
        node.attributes["memory"] = "16";
        node.attributes["latency"] = "10";

        NS_TEST_ASSERT_MSG_EQ(core->RegisterCapability(node),
                              true,
                              "Edge-case node registration should succeed");

        // 1. Missing attribute must fail.
        {
            SixGCapabilityRequest request;
            request.type = "COMPUTING";

            SixGCapabilityConstraint constraint;
            constraint.attribute = "storage";
            constraint.op = SixGConstraintOperator::GREATER_EQUAL;
            constraint.value = "16";

            request.constraints.push_back(constraint);

            auto matches = core->DiscoverCapabilities(request);

            NS_TEST_ASSERT_MSG_EQ(matches.size(),
                                  0u,
                                  "Missing attribute should reject capability");
        }

        // 2. Invalid numeric value must fail.
        {
            SixGCapability invalidNode = node;
            invalidNode.nodeId = "Node-Invalid";
            invalidNode.attributes["memory"] = "unknown";

            NS_TEST_ASSERT_MSG_EQ(core->RegisterCapability(invalidNode),
                                  true,
                                  "Invalid-value node registration should succeed");

            SixGCapabilityRequest request;
            request.type = "COMPUTING";

            SixGCapabilityConstraint constraint;
            constraint.attribute = "memory";
            constraint.op = SixGConstraintOperator::GREATER_EQUAL;
            constraint.value = "16";

            request.constraints.push_back(constraint);

            auto matches = core->DiscoverCapabilities(request);

            NS_TEST_ASSERT_MSG_EQ(matches.size(),
                                  1u,
                                  "Only the valid numeric capability should match");

            NS_TEST_ASSERT_MSG_EQ(matches[0].nodeId,
                                  "Node-Edge",
                                  "Invalid numeric value must be rejected");
        }

        // 3. Boundary: 16 >= 16 must pass.
        {
            SixGCapabilityRequest request;
            request.type = "COMPUTING";

            SixGCapabilityConstraint constraint;
            constraint.attribute = "memory";
            constraint.op = SixGConstraintOperator::GREATER_EQUAL;
            constraint.value = "16";

            request.constraints.push_back(constraint);

            auto matches = core->DiscoverCapabilities(request);

            NS_TEST_ASSERT_MSG_EQ(matches.size(),
                                  1u,
                                  "Exact GREATER_EQUAL boundary should pass");
        }

        // 4. Strict comparison: 16 > 16 must fail.
        {
            SixGCapabilityRequest request;
            request.type = "COMPUTING";

            SixGCapabilityConstraint constraint;
            constraint.attribute = "memory";
            constraint.op = SixGConstraintOperator::GREATER_THAN;
            constraint.value = "16";

            request.constraints.push_back(constraint);

            auto matches = core->DiscoverCapabilities(request);

            NS_TEST_ASSERT_MSG_EQ(matches.size(),
                                  0u,
                                  "Exact GREATER_THAN boundary should fail");
        }

        // 5. NOT_EQUAL: GPU != CPU must pass.
        {
            SixGCapabilityRequest request;
            request.type = "COMPUTING";

            SixGCapabilityConstraint constraint;
            constraint.attribute = "accelerator";
            constraint.op = SixGConstraintOperator::NOT_EQUAL;
            constraint.value = "CPU";

            request.constraints.push_back(constraint);

            auto matches = core->DiscoverCapabilities(request);

            NS_TEST_ASSERT_MSG_EQ(matches.size(),
                                  2u,
                                  "Both GPU capabilities should satisfy NOT_EQUAL");
        }
    }
};

class SixGDynamicCapabilityDiscoveryTestCase : public TestCase
{
  public:
    SixGDynamicCapabilityDiscoveryTestCase()
        : TestCase("Test dynamic capability discovery across simulation time")
    {
    }

  private:
    Ptr<SixGCore> m_core;
    SixGCapabilityProvider m_provider;
    SixGCapabilityRequest m_request;

    void CheckInitialDiscovery()
    {
        auto matches = m_core->DiscoverCapabilities(m_request);

        NS_TEST_ASSERT_MSG_EQ(matches.size(),
                              1u,
                              "Capability should initially satisfy the request");

        NS_TEST_ASSERT_MSG_EQ_TOL(matches[0].availableResource,
                                  100.0,
                                  1e-9,
                                  "Initial available resource should be 100");
    }

    void ConsumeResources()
    {
        NS_TEST_ASSERT_MSG_EQ(m_provider.ConsumeResource(50.0),
                              true,
                              "Resource consumption should succeed");

        auto matches = m_core->DiscoverCapabilities(m_request);

        NS_TEST_ASSERT_MSG_EQ(matches.size(),
                              0u,
                              "Capability should no longer satisfy 60-resource request");
    }

    void ReleaseResources()
    {
        NS_TEST_ASSERT_MSG_EQ(m_provider.ReleaseResource(30.0),
                              true,
                              "Resource release should succeed");

        auto matches = m_core->DiscoverCapabilities(m_request);

        NS_TEST_ASSERT_MSG_EQ(matches.size(),
                              1u,
                              "Capability should become discoverable again");

        NS_TEST_ASSERT_MSG_EQ_TOL(matches[0].availableResource,
                                  80.0,
                                  1e-9,
                                  "Available resource should be restored to 80");
    }

    void DoRun() override
    {
        m_core = CreateObject<SixGCore>();

        m_provider.SetCore(m_core);
        m_provider.SetNodeId("Dynamic-GPU");
        m_provider.SetCapabilityType("COMPUTING");
        m_provider.SetCapacity(100.0);
        m_provider.SetAvailableResource(100.0);
        m_provider.SetAvailable(true);
        m_provider.SetAttribute("accelerator", "GPU");

        NS_TEST_ASSERT_MSG_EQ(m_provider.Register(),
                              true,
                              "Dynamic provider registration should succeed");

        m_request.type = "COMPUTING";
        m_request.minimumResource = 60.0;
        m_request.requireAvailable = true;

        SixGCapabilityConstraint accelerator;
        accelerator.attribute = "accelerator";
        accelerator.op = SixGConstraintOperator::EQUAL;
        accelerator.value = "GPU";

        m_request.constraints.push_back(accelerator);

        Simulator::Schedule(Seconds(0.0),
                            &SixGDynamicCapabilityDiscoveryTestCase::CheckInitialDiscovery,
                            this);

        Simulator::Schedule(Seconds(1.0),
                            &SixGDynamicCapabilityDiscoveryTestCase::ConsumeResources,
                            this);

        Simulator::Schedule(Seconds(2.0),
                            &SixGDynamicCapabilityDiscoveryTestCase::ReleaseResources,
                            this);

        Simulator::Stop(Seconds(3.0));
        Simulator::Run();
        Simulator::Destroy();
    }
};


class SixGStateMonitorProviderTestCase : public TestCase
{
  public:
    SixGStateMonitorProviderTestCase()
        : TestCase("Verify capability provider derives state from SixGStateMonitor")
    {
    }

  private:
    void DoRun() override
    {
        auto core = CreateObject<SixGCore>();
        core->Initialize();

        auto monitor = CreateObject<SixGStateMonitor>();
        monitor->SetNodeId("Edge-01");
        monitor->SetComputeCapacity(100.0);
        monitor->SetAvailableCompute(70.0);
        monitor->SetConnected(true);

        auto provider = CreateObject<SixGCapabilityProvider>();
        provider->SetCore(core);
        provider->SetStateMonitor(monitor);
        provider->SetCapabilityType("COMPUTING");

        NS_TEST_EXPECT_MSG_EQ(provider->UpdateFromState(),
                              true,
                              "Provider should successfully update from monitor");

        const auto capability = provider->GetCapability();

        NS_TEST_EXPECT_MSG_EQ(capability.nodeId,
                              std::string("Edge-01"),
                              "Provider should inherit node ID from monitor");

        NS_TEST_EXPECT_MSG_EQ_TOL(capability.capacity,
                                  100.0,
                                  1e-9,
                                  "Provider should inherit compute capacity");

        NS_TEST_EXPECT_MSG_EQ_TOL(capability.availableResource,
                                  70.0,
                                  1e-9,
                                  "Provider should inherit available compute");

        NS_TEST_EXPECT_MSG_EQ(capability.available,
                              true,
                              "Provider should inherit connectivity state");

        const auto registered = core->GetCapabilities();

        NS_TEST_EXPECT_MSG_EQ(registered.size(),
                              1u,
                              "Exactly one capability should be registered");

        NS_TEST_EXPECT_MSG_EQ(registered[0].nodeId,
                              std::string("Edge-01"),
                              "Registry should contain monitored node");

        NS_TEST_EXPECT_MSG_EQ_TOL(registered[0].availableResource,
                                  70.0,
                                  1e-9,
                                  "Registry should contain monitored resource");
    }
};



class SixGCommunicationStateUpdateTestCase : public TestCase
{
  public:
    SixGCommunicationStateUpdateTestCase()
        : TestCase("Communication capability follows dynamic state")
    {
    }

  private:
    void DoRun() override
    {
        Ptr<SixGCore> core = CreateObject<SixGCore>();
        core->Initialize();

        Ptr<SixGStateMonitor> monitor =
            CreateObject<SixGStateMonitor>();

        monitor->SetNodeId("gNB-Dynamic");
        monitor->SetCommunicationCapacity(100.0);
        monitor->SetAvailableCommunication(80.0);
        monitor->SetConnected(true);

        Ptr<SixGCapabilityProvider> provider =
            CreateObject<SixGCapabilityProvider>();

        provider->SetCore(core);
        provider->SetStateMonitor(monitor);
        provider->SetCapabilityType("COMMUNICATION");

        NS_TEST_ASSERT_MSG_EQ(
            provider->UpdateFromState(),
            true,
            "Initial communication state update should succeed");

        auto capabilities = core->GetCapabilities();

        NS_TEST_ASSERT_MSG_EQ(
            capabilities.size(),
            1u,
            "Exactly one communication capability should exist");

        NS_TEST_ASSERT_MSG_EQ_TOL(
            capabilities[0].capacity,
            100.0,
            1e-9,
            "Initial communication capacity should be 100");

        NS_TEST_ASSERT_MSG_EQ_TOL(
            capabilities[0].availableResource,
            80.0,
            1e-9,
            "Initial available communication resource should be 80");

        NS_TEST_ASSERT_MSG_EQ(
            capabilities[0].available,
            true,
            "Initially connected communication capability should be available");

        monitor->SetCommunicationCapacity(100.0);
        monitor->SetAvailableCommunication(35.0);
        monitor->SetConnected(false);

        NS_TEST_ASSERT_MSG_EQ(
            provider->UpdateFromState(),
            true,
            "Dynamic communication state update should succeed");

        capabilities = core->GetCapabilities();

        NS_TEST_ASSERT_MSG_EQ_TOL(
            capabilities[0].availableResource,
            35.0,
            1e-9,
            "Updated available communication resource should be 35");

        NS_TEST_ASSERT_MSG_EQ(
            capabilities[0].available,
            false,
            "Disconnected communication capability should become unavailable");

        monitor->SetAvailableCommunication(70.0);
        monitor->SetConnected(true);

        NS_TEST_ASSERT_MSG_EQ(
            provider->UpdateFromState(),
            true,
            "Communication recovery update should succeed");

        capabilities = core->GetCapabilities();

        NS_TEST_ASSERT_MSG_EQ_TOL(
            capabilities[0].availableResource,
            70.0,
            1e-9,
            "Recovered communication resource should be 70");

        NS_TEST_ASSERT_MSG_EQ(
            capabilities[0].available,
            true,
            "Reconnected communication capability should become available");

        Simulator::Destroy();
    }
};


class SixGCommunicationQualityTestCase : public TestCase
{
  public:
    SixGCommunicationQualityTestCase()
        : TestCase("Communication resource follows quality")
    {
    }

  private:
    void DoRun() override
    {
        Ptr<SixGStateMonitor> monitor =
            CreateObject<SixGStateMonitor>();

        monitor->SetCommunicationCapacity(100.0);

        monitor->SetCommunicationQuality(1.0);
        NS_TEST_ASSERT_MSG_EQ(
            monitor->UpdateCommunicationFromQuality(),
            true,
            "Quality update should succeed");

        NS_TEST_ASSERT_MSG_EQ_TOL(
            monitor->GetAvailableCommunication(),
            100.0,
            1e-9,
            "Quality 1.0 should provide full communication resource");

        monitor->SetCommunicationQuality(0.5);
        NS_TEST_ASSERT_MSG_EQ(
            monitor->UpdateCommunicationFromQuality(),
            true,
            "Quality update should succeed");

        NS_TEST_ASSERT_MSG_EQ_TOL(
            monitor->GetAvailableCommunication(),
            50.0,
            1e-9,
            "Quality 0.5 should provide half communication resource");

        monitor->SetCommunicationQuality(0.2);
        NS_TEST_ASSERT_MSG_EQ(
            monitor->UpdateCommunicationFromQuality(),
            true,
            "Quality update should succeed");

        NS_TEST_ASSERT_MSG_EQ_TOL(
            monitor->GetAvailableCommunication(),
            20.0,
            1e-9,
            "Quality 0.2 should provide 20 percent communication resource");

        monitor->SetCommunicationQuality(0.8);
        NS_TEST_ASSERT_MSG_EQ(
            monitor->UpdateCommunicationFromQuality(),
            true,
            "Quality recovery should succeed");

        NS_TEST_ASSERT_MSG_EQ_TOL(
            monitor->GetAvailableCommunication(),
            80.0,
            1e-9,
            "Quality recovery to 0.8 should restore 80 percent resource");
    }
};

class SixGMobilityStateTestCase : public TestCase
{
  public:
    SixGMobilityStateTestCase()
        : TestCase("State monitor follows simulated mobility")
    {
    }

  private:
    void DoRun() override
    {
        Ptr<ConstantVelocityMobilityModel> mobility =
            CreateObject<ConstantVelocityMobilityModel>();

        mobility->SetPosition(Vector(0.0, 0.0, 0.0));
        mobility->SetVelocity(Vector(5.0, 0.0, 0.0));

        Ptr<SixGStateMonitor> monitor =
            CreateObject<SixGStateMonitor>();

        monitor->SetNodeId("Mobile-UE");
        monitor->SetMobilityModel(mobility);

        Simulator::Schedule(
            Seconds(0.0),
            &SixGStateMonitor::UpdateFromMobility,
            monitor);

        Simulator::Schedule(
            Seconds(2.0),
            &SixGStateMonitor::UpdateFromMobility,
            monitor);

        Simulator::Stop(Seconds(3.0));
        Simulator::Run();

        Vector position = monitor->GetPosition();

        NS_TEST_ASSERT_MSG_EQ_TOL(
            position.x,
            10.0,
            1e-9,
            "Monitor should observe the UE moving 10 meters in 2 seconds");

        NS_TEST_ASSERT_MSG_EQ_TOL(
            position.y,
            0.0,
            1e-9,
            "UE should not move along Y");

        Simulator::Destroy();
    }
};

class SixGDynamicStateUpdateTestCase : public TestCase
{
  public:
    SixGDynamicStateUpdateTestCase()
        : TestCase("Verify capability follows changing simulation state")
    {
    }

  private:
    void DoRun() override
    {
        auto core = CreateObject<SixGCore>();
        core->Initialize();

        auto monitor = CreateObject<SixGStateMonitor>();
        monitor->SetNodeId("Edge-01");
        monitor->SetComputeCapacity(100.0);
        monitor->SetAvailableCompute(80.0);
        monitor->SetConnected(true);

        auto provider = CreateObject<SixGCapabilityProvider>();
        provider->SetCore(core);
        provider->SetStateMonitor(monitor);
        provider->SetCapabilityType("COMPUTING");

        NS_TEST_EXPECT_MSG_EQ(provider->UpdateFromState(),
                              true,
                              "Initial state update should succeed");

        monitor->SetAvailableCompute(35.0);

        NS_TEST_EXPECT_MSG_EQ(provider->UpdateFromState(),
                              true,
                              "Capability should update from changed state");

        auto capabilities = core->GetCapabilities();

        NS_TEST_EXPECT_MSG_EQ(capabilities.size(),
                              1u,
                              "Registry should still contain one capability");

        NS_TEST_EXPECT_MSG_EQ_TOL(capabilities[0].availableResource,
                                  35.0,
                                  1e-9,
                                  "Registry should contain the new available resource");
    }
};

class SixGIntegratedDynamicComputeTestCase : public TestCase
{
  public:
    SixGIntegratedDynamicComputeTestCase()
        : TestCase("Verify time-driven compute task updates capability state")
    {
    }

  private:
    void DoRun() override
    {
        auto core = CreateObject<SixGCore>();
        core->Initialize();

        auto monitor = CreateObject<SixGStateMonitor>();
        monitor->SetNodeId("Edge-Dynamic");
        monitor->SetComputeCapacity(100.0);
        monitor->SetAvailableCompute(80.0);
        monitor->SetConnected(true);

        auto provider = CreateObject<SixGCapabilityProvider>();
        provider->SetCore(core);
        provider->SetStateMonitor(monitor);
        provider->SetCapabilityType("COMPUTING");

        NS_TEST_EXPECT_MSG_EQ(provider->UpdateFromState(),
                              true,
                              "Initial capability state should be created");

        auto task = CreateObject<SixGComputeTask>();
        task->SetProvider(provider);
        task->SetRequiredResource(50.0);

        Simulator::Schedule(Seconds(2.0),
                            &SixGComputeTask::Start,
                            task);

        Simulator::Schedule(Seconds(5.0),
                            &SixGComputeTask::Finish,
                            task);

        Simulator::Stop(Seconds(6.0));
        Simulator::Run();

        auto capabilities = core->GetCapabilities();

        NS_TEST_EXPECT_MSG_EQ(capabilities.size(),
                              1u,
                              "Exactly one capability should remain registered");

        NS_TEST_EXPECT_MSG_EQ_TOL(capabilities[0].availableResource,
                                  80.0,
                                  1e-9,
                                  "Compute resource should be restored after task completion");

        NS_TEST_EXPECT_MSG_EQ(task->IsRunning(),
                              false,
                              "Task should be finished");

        NS_TEST_EXPECT_MSG_EQ_TOL(monitor->GetAvailableCompute(),
                                  80.0,
                                  1e-9,
                                  "State monitor should contain restored compute");

        Simulator::Destroy();
    }
};

class SixGCapabilityProviderTestCase : public TestCase
{
  public:
    SixGCapabilityProviderTestCase()
        : TestCase("Test 6G capability provider resource lifecycle")
    {
    }

  private:
    void DoRun() override
    {
        Ptr<SixGCore> core = CreateObject<SixGCore>();

        Ptr<SixGCapabilityProvider> provider =
            CreateObject<SixGCapabilityProvider>();

        provider->SetCore(core);
        provider->SetNodeId("Node-A");
        provider->SetCapabilityType("COMPUTING");
        provider->SetCapacity(100.0);
        provider->SetAvailable(true);

        provider->SetAttribute("accelerator", "GPU");
        provider->SetAttribute("memory", "16GB");

        NS_TEST_ASSERT_MSG_EQ(provider->GetAttribute("accelerator"),
                              "GPU",
                              "Accelerator attribute should be GPU");

        NS_TEST_ASSERT_MSG_EQ(provider->GetAttribute("memory"),
                              "16GB",
                              "Memory attribute should be 16GB");

        NS_TEST_ASSERT_MSG_EQ(provider->GetAttribute("missing"),
                              "",
                              "Missing attribute should return an empty string");

        NS_TEST_ASSERT_MSG_EQ(provider->Register(),
                              true,
                              "Provider registration should succeed");

        auto capability = provider->GetCapability();

        NS_TEST_ASSERT_MSG_EQ_TOL(capability.capacity,
                                  100.0,
                                  1e-9,
                                  "Initial capacity should be 100");

        NS_TEST_ASSERT_MSG_EQ_TOL(capability.availableResource,
                                  100.0,
                                  1e-9,
                                  "Initial available resource should be 100");

        NS_TEST_ASSERT_MSG_EQ(provider->ConsumeResource(60.0),
                              true,
                              "Resource consumption should succeed");

        capability = provider->GetCapability();

        NS_TEST_ASSERT_MSG_EQ_TOL(capability.capacity,
                                  100.0,
                                  1e-9,
                                  "Capacity should remain 100");

        NS_TEST_ASSERT_MSG_EQ_TOL(capability.availableResource,
                                  40.0,
                                  1e-9,
                                  "Available resource should become 40");

        auto registryCapabilities = core->GetCapabilities();

        NS_TEST_ASSERT_MSG_EQ(registryCapabilities.size(),
                              1u,
                              "Registry should contain one capability");

        NS_TEST_ASSERT_MSG_EQ_TOL(registryCapabilities[0].capacity,
                                  100.0,
                                  1e-9,
                                  "Registry capacity should remain 100");

        NS_TEST_ASSERT_MSG_EQ_TOL(registryCapabilities[0].availableResource,
                                  40.0,
                                  1e-9,
                                  "Registry available resource should be 40");

        NS_TEST_ASSERT_MSG_EQ(provider->ConsumeResource(50.0),
                              false,
                              "Cannot consume more than available resource");

        NS_TEST_ASSERT_MSG_EQ(provider->ReleaseResource(20.0),
                              true,
                              "Resource release should succeed");

        capability = provider->GetCapability();

        NS_TEST_ASSERT_MSG_EQ_TOL(capability.availableResource,
                                  60.0,
                                  1e-9,
                                  "Available resource should become 60");

        NS_TEST_ASSERT_MSG_EQ(provider->ReleaseResource(50.0),
                              false,
                              "Cannot release beyond total capacity");
    }
};

class SixGComputeTaskTestCase : public TestCase
{
  public:
    SixGComputeTaskTestCase()
        : TestCase("Test 6G compute task resource lifecycle")
    {
    }

  private:
    void DoRun() override
    {
        Ptr<SixGCore> core = CreateObject<SixGCore>();

        Ptr<SixGResource> computeResource =
            CreateObject<SixGResource>();
        computeResource->SetType("COMPUTING");
        computeResource->SetCapacity(100.0);
        computeResource->SetAvailable(100.0);

        Ptr<SixGCapabilityProvider> provider =
            CreateObject<SixGCapabilityProvider>();

        provider->SetCore(core);
        provider->SetNodeId("Node-A");
        provider->SetCapabilityType("COMPUTING");
        provider->SetResource(computeResource);

        NS_TEST_ASSERT_MSG_EQ(provider->Register(),
                              true,
                              "Provider registration should succeed");

        Ptr<SixGComputeTask> task =
            CreateObject<SixGComputeTask>();

        task->SetProvider(provider);
        task->SetComputeResource(computeResource);
        task->SetRequiredResource(60.0);

        NS_TEST_ASSERT_MSG_EQ(task->IsRunning(),
                              false,
                              "Task should initially be stopped");

        NS_TEST_ASSERT_MSG_EQ(task->Start(),
                              true,
                              "Task should start successfully");

        NS_TEST_ASSERT_MSG_EQ(task->IsRunning(),
                              true,
                              "Task should be running");

        auto capability = provider->GetCapability();

        NS_TEST_ASSERT_MSG_EQ_TOL(capability.availableResource,
                                  40.0,
                                  1e-9,
                                  "Available resource should be 40 after task starts");

        NS_TEST_ASSERT_MSG_EQ(task->Start(),
                              false,
                              "Running task should not start again");

        NS_TEST_ASSERT_MSG_EQ(task->Finish(),
                              true,
                              "Task should finish successfully");

        NS_TEST_ASSERT_MSG_EQ(task->IsRunning(),
                              false,
                              "Task should no longer be running");

        capability = provider->GetCapability();

        NS_TEST_ASSERT_MSG_EQ_TOL(capability.availableResource,
                                  100.0,
                                  1e-9,
                                  "Available resource should be restored to 100");
    }
};

class SixGHeterogeneousCapabilityTestCase : public TestCase
{
  public:
    SixGHeterogeneousCapabilityTestCase()
        : TestCase("Test heterogeneous 6G capability discovery")
    {
    }

  private:
    void DoRun() override
    {
        Ptr<SixGCore> core = CreateObject<SixGCore>();

        SixGCapability computeA;
        computeA.nodeId = "Node-A";
        computeA.type = "COMPUTING";
        computeA.capacity = 100.0;
        computeA.availableResource = 100.0;
        computeA.available = true;

        SixGCapability computeB;
        computeB.nodeId = "Node-B";
        computeB.type = "COMPUTING";
        computeB.capacity = 80.0;
        computeB.availableResource = 80.0;
        computeB.available = true;

        SixGCapability computeC;
        computeC.nodeId = "Node-C";
        computeC.type = "COMPUTING";
        computeC.capacity = 40.0;
        computeC.availableResource = 40.0;
        computeC.available = true;

        SixGCapability sensingA;
        sensingA.nodeId = "Node-A";
        sensingA.type = "SENSING";
        sensingA.capacity = 200.0;
        sensingA.availableResource = 200.0;
        sensingA.available = true;

        NS_TEST_ASSERT_MSG_EQ(core->RegisterCapability(computeA),
                              true,
                              "Node-A computing registration should succeed");

        NS_TEST_ASSERT_MSG_EQ(core->RegisterCapability(computeB),
                              true,
                              "Node-B computing registration should succeed");

        NS_TEST_ASSERT_MSG_EQ(core->RegisterCapability(computeC),
                              true,
                              "Node-C computing registration should succeed");

        NS_TEST_ASSERT_MSG_EQ(core->RegisterCapability(sensingA),
                              true,
                              "Node-A sensing registration should succeed");

        SixGCapabilityRequest request;
        request.type = "COMPUTING";
        request.minimumResource = 50.0;
        request.requireAvailable = true;

        auto matches = core->DiscoverCapabilities(request);

        NS_TEST_ASSERT_MSG_EQ(matches.size(),
                              2u,
                              "Two computing capabilities should match");

        for (const auto& capability : matches)
        {
            NS_TEST_ASSERT_MSG_EQ(capability.type,
                                  "COMPUTING",
                                  "Only computing capabilities should match");

            NS_TEST_ASSERT_MSG_EQ(capability.available,
                                  true,
                                  "Matched capability should be available");

            NS_TEST_ASSERT_MSG_EQ(capability.availableResource >= 50.0,
                                  true,
                                  "Matched capability should have sufficient available resource");
        }
    }
};

class SixGCapabilityMatcherTestCase : public TestCase
{
  public:
    SixGCapabilityMatcherTestCase()
        : TestCase("Test 6G capability matching and ranking")
    {
    }

  private:
    void DoRun() override
    {
        Ptr<SixGCapabilityMatcher> matcher =
            CreateObject<SixGCapabilityMatcher>();

        SixGCapability nodeA;
        nodeA.nodeId = "Node-A";
        nodeA.type = "COMPUTING";
        nodeA.capacity = 100.0;
        nodeA.availableResource = 100.0;
        nodeA.available = true;

        SixGCapability nodeB;
        nodeB.nodeId = "Node-B";
        nodeB.type = "COMPUTING";
        nodeB.capacity = 80.0;
        nodeB.availableResource = 80.0;
        nodeB.available = true;

        SixGCapability nodeC;
        nodeC.nodeId = "Node-C";
        nodeC.type = "COMPUTING";
        nodeC.capacity = 40.0;
        nodeC.availableResource = 40.0;
        nodeC.available = true;

        SixGCapability nodeD;
        nodeD.nodeId = "Node-D";
        nodeD.type = "COMPUTING";
        nodeD.capacity = 120.0;
        nodeD.availableResource = 120.0;
        nodeD.available = false;

        std::vector<SixGCapability> candidates = {
            nodeC,
            nodeD,
            nodeB,
            nodeA};

        SixGCapabilityRequest request;
        request.type = "COMPUTING";
        request.minimumResource = 50.0;
        request.requireAvailable = true;

        auto matches = matcher->Match(request, candidates);

        NS_TEST_ASSERT_MSG_EQ(matches.size(),
                              2u,
                              "Two candidates should satisfy the request");

        NS_TEST_ASSERT_MSG_EQ(matches[0].nodeId,
                              "Node-A",
                              "Node-A should rank first");

        NS_TEST_ASSERT_MSG_EQ(matches[1].nodeId,
                              "Node-B",
                              "Node-B should rank second");

        NS_TEST_ASSERT_MSG_EQ_TOL(matches[0].availableResource,
                                  100.0,
                                  1e-9,
                                  "Node-A should have 100 available resource");

        NS_TEST_ASSERT_MSG_EQ_TOL(matches[1].availableResource,
                                  80.0,
                                  1e-9,
                                  "Node-B should have 80 available resource");
    }
};

class SixGCoreTestSuite : public TestSuite
{
  public:
    SixGCoreTestSuite()
        : TestSuite("6g-core", Type::UNIT)
    {
        AddTestCase(new SixGCoreTestCase1);
        AddTestCase(new SixGStateMonitorProviderTestCase);
        AddTestCase(new SixGMobilityStateTestCase);
        AddTestCase(new SixGCommunicationQualityTestCase);
        AddTestCase(new SixGCommunicationStateUpdateTestCase);
        AddTestCase(new SixGIntegratedDynamicComputeTestCase);
        AddTestCase(new SixGDynamicStateUpdateTestCase);
        AddTestCase(new SixGCapabilityProviderTestCase);
        AddTestCase(new SixGCapabilityStateTestCase);
        AddTestCase(new SixGCapabilityFreshnessTestCase);
        AddTestCase(new SixGFreshDiscoveryTestCase);
        AddTestCase(new SixGCapabilityTemporalTestCase);
        AddTestCase(new SixGLifecycleDiscoveryTestCase);
    AddTestCase(new SixGConstraintDiscoveryTestCase);
    AddTestCase(new SixGConstraintEdgeCaseTestCase);
    AddTestCase(new SixGDynamicCapabilityDiscoveryTestCase);
        AddTestCase(new SixGAttributeDiscoveryTestCase);
        AddTestCase(new SixGComputeTaskTestCase);
        AddTestCase(new SixGHeterogeneousCapabilityTestCase);
        AddTestCase(new SixGCapabilityMatcherTestCase);
    }
};

static SixGCoreTestSuite g_sixGCoreTestSuite;

} // namespace ns3
