build_lib_example(
    NAME 6g-device-example

    SOURCE_FILES
        6g-device-example.cc

    LIBRARIES_TO_LINK
        ${libcore}
        ${libnetwork}
        ns3::6g-capabilities
        ${lib6g_devices}
)
