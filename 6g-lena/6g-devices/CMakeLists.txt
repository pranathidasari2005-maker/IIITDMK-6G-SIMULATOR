set(examples_as_tests_sources)

if(${ENABLE_EXAMPLES})
    set(examples_as_tests_sources
    )
endif()

build_lib(
    LIBNAME 6g-devices

    SOURCE_FILES
        model/6g-net-device.cc
        helper/6g-device-helper.cc

    HEADER_FILES
        model/6g-net-device.h
        helper/6g-device-helper.h

    LIBRARIES_TO_LINK
        ${libcore}
        ${libnetwork}
        ns3::6g-capabilities

    TEST_SOURCES
        test/6g-device-test-suite.cc
        ${examples_as_tests_sources}
)
