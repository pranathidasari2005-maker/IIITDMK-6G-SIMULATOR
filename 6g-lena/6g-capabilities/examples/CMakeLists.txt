build_lib_example(
    NAME 6g-capability-registry-example

    SOURCE_FILES
        6g-capability-registry-example.cc

    LIBRARIES_TO_LINK
        ${libcore}
        ${libnetwork}
        ${lib6g_capabilities}
)
