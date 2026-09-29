build_lib_example(
    NAME 6g-core-example
    SOURCE_FILES 6g-core-example.cc
    LIBRARIES_TO_LINK ${lib6g-core} ns3::network-nodes
)

build_lib_example(
    NAME 6g-capability-experiment
    SOURCE_FILES 6g-capability-experiment.cc
    LIBRARIES_TO_LINK ${lib6g-core} ns3::network-nodes
)

build_lib_example(
    NAME 6g-capability-constraint-experiment
    SOURCE_FILES 6g-capability-constraint-experiment.cc
    LIBRARIES_TO_LINK ${lib6g-core}
)

build_lib_example(
    NAME 6g-capability-freshness-experiment
    SOURCE_FILES 6g-capability-freshness-experiment.cc
    LIBRARIES_TO_LINK ${lib6g-core}
)

build_lib_example(
    NAME 6g-resource-stress-experiment
    SOURCE_FILES 6g-resource-stress-experiment.cc
    LIBRARIES_TO_LINK ${lib6g-core} ns3::network-nodes
)

build_lib_example(
    NAME 6g-freshness-experiment
    SOURCE_FILES 6g-freshness-experiment.cc
    LIBRARIES_TO_LINK ${lib6g-core}
)

build_lib_example(
    NAME 6g-capability-scalability-experiment
    SOURCE_FILES 6g-capability-scalability-experiment.cc
    LIBRARIES_TO_LINK ${lib6g-core}
)


build_lib_example(
    NAME 6g-service-placement-experiment
    SOURCE_FILES 6g-service-placement-experiment.cc
    LIBRARIES_TO_LINK ${lib6g-core} ns3::network-nodes
)

build_lib_example(
    NAME 6g-capability-update-experiment
    SOURCE_FILES 6g-capability-update-experiment.cc
    LIBRARIES_TO_LINK ${lib6g-core}
)
