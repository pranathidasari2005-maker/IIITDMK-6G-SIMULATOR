set(examples_as_tests_sources)

if(${ENABLE_EXAMPLES})
    set(examples_as_tests_sources
    )
endif()

build_lib(
    LIBNAME 6g-capabilities

    SOURCE_FILES
        model/6g-capability.cc
        model/6g-capability-registry.cc

    HEADER_FILES
        model/6g-capability.h
        model/6g-capability-registry.h

    LIBRARIES_TO_LINK
        ${libcore}
        ${libnetwork}

    TEST_SOURCES
        test/6g-capability-registry-test-suite.cc
        ${examples_as_tests_sources}
)
