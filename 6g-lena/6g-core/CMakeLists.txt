set(examples_as_tests_sources)
if(${ENABLE_EXAMPLES})
    set(examples_as_tests_sources
        #test/6g-core-examples-test-suite.cc
        )
endif()

build_lib(
    LIBNAME 6g-core
    SOURCE_FILES model/6g-resource.cc
                 model/6g-core.cc
                 model/6g-state-monitor.cc
                 model/6g-capability-provider.cc
                 model/6g-compute-task.cc
                 model/6g-capability-matcher.cc
                 helper/6g-core-helper.cc
    HEADER_FILES model/6g-resource.h
                 model/6g-core.h
                 model/6g-state-monitor.h
                 model/6g-capability-provider.h
                 model/6g-compute-task.h
                 model/6g-capability-matcher.h
                 helper/6g-core-helper.h
    LIBRARIES_TO_LINK ${libcore}
                      ${libmobility}
    TEST_SOURCES test/6g-core-test-suite.cc
                 test/6g-resource-test-suite.cc
                 ${examples_as_tests_sources}
)
