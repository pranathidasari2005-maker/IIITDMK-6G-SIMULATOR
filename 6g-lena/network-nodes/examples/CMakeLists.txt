build_lib_example(
    NAME network-nodes-example
    SOURCE_FILES network-nodes-example.cc
    LIBRARIES_TO_LINK ${libnetwork-nodes}
                      ${lib6g-core}
)

build_lib_example(
    NAME 6g-basic-network-example
    SOURCE_FILES 6g-basic-network-example.cc
    LIBRARIES_TO_LINK ${libnetwork-nodes}
)

build_lib_example(
    NAME 6g-communication-computing-example
    SOURCE_FILES 6g-communication-computing-example.cc
    LIBRARIES_TO_LINK ${libnetwork-nodes}
)

build_lib_example(
    NAME 6g-heterogeneous-multi-node-example
    SOURCE_FILES 6g-heterogeneous-multi-node-example.cc
    LIBRARIES_TO_LINK ${libnetwork-nodes}
)

build_lib_example(
    NAME 6g-service-placement-example
    SOURCE_FILES 6g-service-placement-example.cc
    LIBRARIES_TO_LINK ${libnetwork-nodes}
)

build_lib_example(
    NAME 6g-mobility-handover-example
    SOURCE_FILES 6g-mobility-handover-example.cc
    LIBRARIES_TO_LINK ${libnetwork-nodes}
)

build_lib_example(
    NAME 6g-dynamic-resource-state-example
    SOURCE_FILES 6g-dynamic-resource-state-example.cc
    LIBRARIES_TO_LINK ${libnetwork-nodes}
)

build_lib_example(
    NAME 6g-end-to-end-session-experiment
    SOURCE_FILES 6g-end-to-end-session-experiment.cc
    LIBRARIES_TO_LINK ${libnetwork-nodes}
)

build_lib_example(
    NAME 6g-communication-computing-load-experiment
    SOURCE_FILES 6g-communication-computing-load-experiment.cc
    LIBRARIES_TO_LINK ${libnetwork-nodes}
)

build_lib_example(
    NAME 6g-heterogeneous-network-experiment
    SOURCE_FILES 6g-heterogeneous-network-experiment.cc
    LIBRARIES_TO_LINK ${libnetwork-nodes}
)

build_lib_example(
    NAME 6g-mobility-handover-experiment
    SOURCE_FILES 6g-mobility-handover-experiment.cc
    LIBRARIES_TO_LINK ${libnetwork-nodes}
)
