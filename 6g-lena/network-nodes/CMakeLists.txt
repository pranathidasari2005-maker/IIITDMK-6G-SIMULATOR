set(examples_as_tests_sources)
if(${ENABLE_EXAMPLES})
    set(examples_as_tests_sources
        #test/network-nodes-examples-test-suite.cc
        )
endif()

build_lib(
    LIBNAME network-nodes
    SOURCE_FILES model/6g-network-node.cc
                 model/6g-ue.cc
                 model/6g-access-node.cc
                 model/6g-edge-node.cc
                 model/6g-service.cc
                 model/6g-service-session.cc
                 helper/network-nodes-helper.cc
    HEADER_FILES model/network-nodes.h
                 model/6g-network-node.h
                 model/6g-ue.h
                 model/6g-access-node.h
                 model/6g-edge-node.h
                 model/6g-service.h
                 model/6g-service-session.h
                 helper/network-nodes-helper.h
    LIBRARIES_TO_LINK ${libcore}
                      ns3::6g-core
                      ${lib6g_capabilities}
    TEST_SOURCES test/network-nodes-test-suite.cc test/6g-service-session-test-suite.cc
                 ${examples_as_tests_sources}
)

