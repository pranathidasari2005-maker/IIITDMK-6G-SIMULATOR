import re
from flask import Flask, render_template, jsonify, request
import subprocess
from pathlib import Path

app = Flask(__name__)

# Project root: ~/6G-Lab/ns-3-dev
PROJECT_ROOT = Path(__file__).resolve().parents[2]

RESOURCE_STRESS_EXECUTABLE = (
    PROJECT_ROOT / "build" / "contrib" / "6g-lena" / "6g-core"
    / "examples" / "ns3.48-6g-resource-stress-experiment-default"
)



CAPABILITY_SCALABILITY_EXECUTABLE = (
    PROJECT_ROOT
    / "build"
    / "contrib"
    / "6g-lena"
    / "6g-core"
    / "examples"
    / "ns3.48-6g-capability-scalability-experiment-default"
)


@app.route("/")
def index():
    return render_template("index.html")


# Capability Registry
CAPABILITY_REGISTRY_EXECUTABLE = (
    PROJECT_ROOT
    / "build"
    / "contrib"
    / "6g-lena"
    / "6g-capabilities"
    / "examples"
    / "ns3.48-6g-capability-registry-example-default"
)

# Capability Update Experiment
CAPABILITY_UPDATE_EXECUTABLE = (
    PROJECT_ROOT
    / "build"
    / "contrib"
    / "6g-lena"
    / "6g-core"
    / "examples"
    / "ns3.48-6g-capability-update-experiment-default"
)

# Capability Constraint Experiment
CAPABILITY_CONSTRAINT_EXECUTABLE = (
    PROJECT_ROOT
    / "build"
    / "contrib"
    / "6g-lena"
    / "6g-core"
    / "examples"
    / "ns3.48-6g-capability-constraint-experiment-default"
)

CAPABILITY_FRESHNESS_EXECUTABLE = (
    PROJECT_ROOT
    / "build"
    / "contrib"
    / "6g-lena"
    / "6g-core"
    / "examples"
    / "ns3.48-6g-capability-freshness-experiment-default"
)

CAPABILITY_EXPERIMENT_EXECUTABLE = (
    PROJECT_ROOT
    / "build"
    / "contrib"
    / "6g-lena"
    / "6g-core"
    / "examples"
    / "ns3.48-6g-capability-experiment-default"
)


@app.route("/api/capabilities/registry")
def capability_registry():

    executable = CAPABILITY_REGISTRY_EXECUTABLE

    if not executable.exists():
        return jsonify({
            "success": False,
            "error": (
                "Capability registry executable not found: "
                + str(executable)
            )
        }), 500

    communication_providers = request.args.get(
        "communicationProviders", "1"
    )
    sensing_providers = request.args.get(
        "sensingProviders", "1"
    )
    computing_providers = request.args.get(
        "computingProviders", "1"
    )
    ai_providers = request.args.get(
        "aiProviders", "1"
    )
    query_type = request.args.get(
        "queryType", "COMPUTING"
    ).upper()

    try:
        provider_values = {
            "communicationProviders": int(communication_providers),
            "sensingProviders": int(sensing_providers),
            "computingProviders": int(computing_providers),
            "aiProviders": int(ai_providers),
        }

        if any(value < 0 for value in provider_values.values()):
            raise ValueError(
                "Provider counts cannot be negative."
            )

        if sum(provider_values.values()) == 0:
            raise ValueError(
                "At least one capability provider is required."
            )

        valid_query_types = {
            "COMMUNICATION",
            "SENSING",
            "COMPUTING",
            "AI"
        }

        if query_type not in valid_query_types:
            raise ValueError(
                "Invalid queryType. Use COMMUNICATION, SENSING, "
                "COMPUTING, or AI."
            )

        command = [
            str(executable),
            f"--communicationProviders={provider_values['communicationProviders']}",
            f"--sensingProviders={provider_values['sensingProviders']}",
            f"--computingProviders={provider_values['computingProviders']}",
            f"--aiProviders={provider_values['aiProviders']}",
            f"--queryType={query_type}",
        ]

        result = subprocess.run(
            command,
            cwd=str(PROJECT_ROOT),
            capture_output=True,
            text=True,
            timeout=120
        )

        output = result.stdout

        capabilities = []

        for line in output.splitlines():

            line = line.strip()

            if "->" not in line:
                continue

            parts = line.split("->")

            if len(parts) != 2:
                continue

            node = parts[0].strip()
            capability_state = parts[1].strip()

            if not node.startswith("Node "):
                continue

            capability_parts = capability_state.split(":")

            if len(capability_parts) != 2:
                continue

            capability = capability_parts[0].strip()
            state = capability_parts[1].strip()

            capabilities.append({
                "node": node,
                "capability": capability,
                "state": state
            })

        discovery_nodes = []

        in_discovery_section = False

        for line in output.splitlines():

            line = line.strip()

            if line.startswith("Discovery Query:"):
                in_discovery_section = True
                continue

            if in_discovery_section:

                if line.startswith("Node ") and "->" in line:
                    discovery_nodes.append(
                        line.split("->")[0].strip()
                    )

                elif line.startswith("Total registered capabilities:"):
                    break

        return jsonify({
            "success": result.returncode == 0,
            "configuration": {
                "communicationProviders":
                    provider_values["communicationProviders"],
                "sensingProviders":
                    provider_values["sensingProviders"],
                "computingProviders":
                    provider_values["computingProviders"],
                "aiProviders":
                    provider_values["aiProviders"],
                "queryType": query_type
            },
            "capabilities": capabilities,
            "total": len(capabilities),
            "discovery": {
                "query": query_type,
                "found": discovery_nodes,
                "count": len(discovery_nodes)
            },
            "output": output,
            "error": result.stderr
        })

    except ValueError as error:

        return jsonify({
            "success": False,
            "error": str(error)
        }), 400

    except subprocess.TimeoutExpired:

        return jsonify({
            "success": False,
            "error": "Capability registry simulation timed out."
        }), 500


# ---------------------------------------------------------------------------
# Framework example executables
# ---------------------------------------------------------------------------

FRAMEWORK_EXECUTABLES = {
    "device-network-nodes": (
        PROJECT_ROOT / "build" / "contrib" / "6g-lena" / "network-nodes"
        / "examples" / "ns3.48-network-nodes-example-default"
    ),
    "core": (
        PROJECT_ROOT / "build" / "contrib" / "6g-lena" / "6g-core"
        / "examples" / "ns3.48-6g-core-example-default"
    ),
    "communication-computing": (
        PROJECT_ROOT / "build" / "contrib" / "6g-lena" / "network-nodes"
        / "examples" / "ns3.48-6g-communication-computing-example-default"
    ),
    "communication-computing-load": (
        PROJECT_ROOT / "build" / "contrib" / "6g-lena" / "network-nodes"
        / "examples" / "ns3.48-6g-communication-computing-load-experiment-default"
    ),
    "heterogeneous-multi-node": (
        PROJECT_ROOT / "build" / "contrib" / "6g-lena" / "network-nodes"
        / "examples" / "ns3.48-6g-heterogeneous-multi-node-example-default"
    ),
    "heterogeneous-network": (
        PROJECT_ROOT / "build" / "contrib" / "6g-lena" / "network-nodes"
        / "examples" / "ns3.48-6g-heterogeneous-network-experiment-default"
    ),
    "dynamic-resource-state": (
        PROJECT_ROOT / "build" / "contrib" / "6g-lena" / "network-nodes"
        / "examples" / "ns3.48-6g-dynamic-resource-state-example-default"
    ),
    "service-placement-experiment": (
        PROJECT_ROOT / "build" / "contrib" / "6g-lena" / "6g-core"
        / "examples" / "ns3.48-6g-service-placement-experiment-default"
    ),
}


@app.route("/api/framework/<name>")
def run_framework_example(name):

    executable = FRAMEWORK_EXECUTABLES.get(name)

    if executable is None:
        return jsonify({
            "success": False,
            "error": "Unknown framework executable."
        }), 404

    if not executable.exists():
        return jsonify({
            "success": False,
            "error": "Executable not found: " + str(executable)
        }), 500

    try:
        result = subprocess.run(
            [
                str(executable),
                f"--minimum-resource={minimum_resource:g}"
            ],
            cwd=str(PROJECT_ROOT),
            capture_output=True,
            text=True,
            timeout=120
        )

        return jsonify({
            "success": result.returncode == 0,
            "experiment": name,
            "output": result.stdout,
            "error": result.stderr,
            "returncode": result.returncode
        })

    except subprocess.TimeoutExpired:
        return jsonify({
            "success": False,
            "experiment": name,
            "output": "",
            "error": "Simulation timed out."
        }), 500



# ---------------------------------------------------------------------------
# Configurable Communication + Computing Load Experiment
# ---------------------------------------------------------------------------

@app.route("/api/experiment/communication-computing-load", methods=["GET", "POST"])
def communication_computing_load():
    executable = (
        PROJECT_ROOT / "build" / "contrib" / "6g-lena"
        / "network-nodes" / "examples"
        / "ns3.48-6g-communication-computing-load-experiment-default"
    )

    if not executable.exists():
        return jsonify({
            "success": False,
            "error": f"Executable not found: {executable}"
        }), 500

    data = request.get_json(silent=True) or request.args

    def get_float(name, default):
        try:
            return float(data.get(name, default))
        except (TypeError, ValueError):
            return float(default)

    communication_capacity = get_float("communicationCapacity", 100)
    computing_capacity = get_float("computingCapacity", 100)
    communication_per_session = get_float("communicationPerSession", 20)
    computing_per_session = get_float("computingPerSession", 20)

    if communication_capacity <= 0 or computing_capacity <= 0:
        return jsonify({
            "success": False,
            "error": "Resource capacities must be greater than zero."
        }), 400

    if communication_per_session <= 0 or computing_per_session <= 0:
        return jsonify({
            "success": False,
            "error": "Per-session resource requirements must be greater than zero."
        }), 400

    try:
        result = subprocess.run(
            [
                str(executable),
                f"--communicationCapacity={communication_capacity}",
                f"--computingCapacity={computing_capacity}",
                f"--communicationPerSession={communication_per_session}",
                f"--computingPerSession={computing_per_session}",
            ],
            cwd=str(PROJECT_ROOT),
            capture_output=True,
            text=True,
            timeout=120
        )

        return jsonify({
            "success": result.returncode == 0,
            "experiment": "communication-computing-load",
            "parameters": {
                "communicationCapacity": communication_capacity,
                "computingCapacity": computing_capacity,
                "communicationPerSession": communication_per_session,
                "computingPerSession": computing_per_session
            },
            "output": result.stdout,
            "error": result.stderr,
            "returncode": result.returncode
        })

    except subprocess.TimeoutExpired:
        return jsonify({
            "success": False,
            "error": "Simulation timed out."
        }), 504

# ---------------------------------------------------------------------------
# Configurable Heterogeneous Network Experiment
# ---------------------------------------------------------------------------

@app.route("/api/experiment/heterogeneous-network")
def heterogeneous_network_experiment():

    executable = FRAMEWORK_EXECUTABLES.get("heterogeneous-network")

    if executable is None:
        return jsonify({
            "success": False,
            "error": "Heterogeneous network executable is not registered."
        }), 500

    if not executable.exists():
        return jsonify({
            "success": False,
            "error": "Executable not found: " + str(executable)
        }), 500

    def get_positive_float(name, default):
        value = request.args.get(name, default)

        try:
            value = float(value)
        except (TypeError, ValueError):
            raise ValueError(f"{name} must be a valid number.")

        if value <= 0:
            raise ValueError(f"{name} must be greater than 0.")

        return value

    try:
        access1_communication = get_positive_float(
            "access1CommunicationCapacity", 100.0
        )
        access2_communication = get_positive_float(
            "access2CommunicationCapacity", 60.0
        )
        edge1_computing = get_positive_float(
            "edge1ComputingCapacity", 120.0
        )
        edge2_computing = get_positive_float(
            "edge2ComputingCapacity", 80.0
        )
        edge3_computing = get_positive_float(
            "edge3ComputingCapacity", 40.0
        )

        command = [
            str(executable),
            f"--access1CommunicationCapacity={access1_communication:g}",
            f"--access2CommunicationCapacity={access2_communication:g}",
            f"--edge1ComputingCapacity={edge1_computing:g}",
            f"--edge2ComputingCapacity={edge2_computing:g}",
            f"--edge3ComputingCapacity={edge3_computing:g}",
        ]

        result = subprocess.run(
            command,
            cwd=str(PROJECT_ROOT),
            capture_output=True,
            text=True,
            timeout=120
        )

        return jsonify({
            "success": result.returncode == 0,
            "experiment": "heterogeneous-network",
            "parameters": {
                "access1_communication_capacity":
                    access1_communication,
                "access2_communication_capacity":
                    access2_communication,
                "edge1_computing_capacity":
                    edge1_computing,
                "edge2_computing_capacity":
                    edge2_computing,
                "edge3_computing_capacity":
                    edge3_computing
            },
            "output": result.stdout,
            "error": result.stderr,
            "returncode": result.returncode
        })

    except ValueError as exc:
        return jsonify({
            "success": False,
            "error": str(exc)
        }), 400

    except subprocess.TimeoutExpired:
        return jsonify({
            "success": False,
            "error": "Heterogeneous network experiment timed out."
        }), 504


# ---------------------------------------------------------------------------
# Configurable Dynamic Resource State Experiment
# ---------------------------------------------------------------------------

@app.route("/api/experiment/dynamic-resource-state")
def dynamic_resource_state_experiment():

    executable = FRAMEWORK_EXECUTABLES.get("dynamic-resource-state")

    if executable is None:
        return jsonify({
            "success": False,
            "error": "Dynamic resource state executable is not registered."
        }), 500

    if not executable.exists():
        return jsonify({
            "success": False,
            "error": "Executable not found: " + str(executable)
        }), 500

    def get_non_negative_float(name, default):
        value = request.args.get(name, default)

        try:
            value = float(value)
        except (TypeError, ValueError):
            raise ValueError(f"{name} must be a valid number.")

        if value < 0:
            raise ValueError(f"{name} must be non-negative.")

        return value

    try:
        capacity = get_non_negative_float(
            "capacity", 100.0
        )

        consumption = get_non_negative_float(
            "consumption", 70.0
        )

        release = get_non_negative_float(
            "release", 40.0
        )

        if capacity <= 0:
            raise ValueError(
                "capacity must be greater than 0."
            )

        if consumption > capacity:
            raise ValueError(
                "consumption must be less than or equal to capacity."
            )

        if release > consumption:
            raise ValueError(
                "release must be less than or equal to consumption."
            )

        command = [
            str(executable),
            f"--capacity={capacity:g}",
            f"--consumption={consumption:g}",
            f"--release={release:g}",
        ]

        result = subprocess.run(
            command,
            cwd=str(PROJECT_ROOT),
            capture_output=True,
            text=True,
            timeout=120
        )

        return jsonify({
            "success": result.returncode == 0,
            "experiment": "dynamic-resource-state",
            "parameters": {
                "capacity": capacity,
                "consumption": consumption,
                "release": release
            },
            "output": result.stdout,
            "error": result.stderr,
            "returncode": result.returncode
        })

    except ValueError as exc:
        return jsonify({
            "success": False,
            "error": str(exc)
        }), 400

    except subprocess.TimeoutExpired:
        return jsonify({
            "success": False,
            "error": "Dynamic resource state experiment timed out."
        }), 504



# ---------------------------------------------------------------------------
# Configurable Capability-Based Service Placement Experiment
# ---------------------------------------------------------------------------

@app.route("/api/experiment/service-placement", methods=["GET", "POST"])
def service_placement_experiment():
    executable = (
        PROJECT_ROOT / "build" / "contrib" / "6g-lena"
        / "6g-core" / "examples"
        / "ns3.48-6g-service-placement-experiment-default"
    )

    if not executable.exists():
        return jsonify({
            "success": False,
            "error": f"Executable not found: {executable}"
        }), 500

    data = request.get_json(silent=True) or request.args

    try:
        repetitions = int(data.get("repetitions", 5))
    except (TypeError, ValueError):
        repetitions = 5

    if repetitions <= 0:
        return jsonify({
            "success": False,
            "error": "Placement requests must be greater than zero."
        }), 400

    try:
        result = subprocess.run(
            [
                str(executable),
                f"--repetitions={repetitions}"
            ],
            cwd=str(PROJECT_ROOT),
            capture_output=True,
            text=True,
            timeout=120
        )

        return jsonify({
            "success": result.returncode == 0,
            "experiment": "service-placement",
            "parameters": {
                "repetitions": repetitions
            },
            "output": result.stdout,
            "error": result.stderr,
            "returncode": result.returncode
        })

    except subprocess.TimeoutExpired:
        return jsonify({
            "success": False,
            "error": "Simulation timed out."
        }), 504

@app.route("/api/status")
def status():
    return jsonify({
        "simulator": "IIITDMK 6G SIMULATOR",
        "engine": "ns-3 / 6G-LENA",
        "status": "ready"
    })


@app.route("/api/experiment/capability-scalability")
def capability_scalability():

    executable = CAPABILITY_SCALABILITY_EXECUTABLE

    if not executable.exists():
        return jsonify({
            "success": False,
            "experiment": "Capability Discovery Scalability",
            "output": "",
            "error": (
                "Simulation executable not found: "
                + str(executable)
            )
        }), 500

    try:

        result = subprocess.run(
            [str(executable)],
            cwd=str(PROJECT_ROOT),
            capture_output=True,
            text=True,
            timeout=120
        )

        return jsonify({
            "success": result.returncode == 0,
            "experiment": "Capability Discovery Scalability",
            "output": result.stdout,
            "error": result.stderr
        })

    except subprocess.TimeoutExpired:

        return jsonify({
            "success": False,
            "experiment": "Capability Discovery Scalability",
            "output": "",
            "error": "Simulation timed out."
        }), 500


# Dynamic Capability Discovery experiment
CAPABILITY_DISCOVERY_EXECUTABLE = (
    PROJECT_ROOT
    / "build"
    / "contrib"
    / "6g-lena"
    / "6g-core"
    / "examples"
    / "ns3.48-6g-capability-experiment-default"
)


@app.route("/api/experiment/capability-discovery")
def capability_discovery():

    executable = CAPABILITY_DISCOVERY_EXECUTABLE

    if not executable.exists():
        return jsonify({
            "success": False,
            "experiment": "Dynamic Capability Discovery",
            "output": "",
            "error": (
                "Simulation executable not found: "
                + str(executable)
            )
        }), 500

    try:

        requests = request.args.get("requests", "10")
        profile = request.args.get("profile", "1")
        updates = request.args.get("updates", "1")

        result = subprocess.run(
            [
                str(executable),
                "--requests=" + requests,
                "--profile=" + profile,
                "--updates=" + updates
            ],
            cwd=str(PROJECT_ROOT),
            capture_output=True,
            text=True,
            timeout=120
        )

        return jsonify({
            "success": result.returncode == 0,
            "experiment": "Dynamic Capability Discovery",
            "output": result.stdout,
            "error": result.stderr
        })

    except subprocess.TimeoutExpired:

        return jsonify({
            "success": False,
            "experiment": "Dynamic Capability Discovery",
            "output": "",
            "error": "Simulation timed out."
        }), 500


# =========================================================
# 6G NETWORK SIMULATION
# =========================================================

NETWORK_EXECUTABLE = (
    PROJECT_ROOT
    / "build"
    / "contrib"
    / "6g-lena"
    / "network-nodes"
    / "examples"
    / "ns3.48-6g-basic-network-example-default"
)

@app.route("/api/network/build", methods=["POST"])
def build_network():

    if not NETWORK_EXECUTABLE.exists():
        return jsonify({
            "success": False,
            "error": "6G basic network executable not found."
        }), 500

    try:
        request_data = request.get_json(silent=True) or {}

        access_nodes = int(request_data.get("access_nodes", 2))
        edge_nodes = int(request_data.get("edge_nodes", 2))
        ues = int(request_data.get("ues", 10))
        services = int(request_data.get("services", 3))

        if (
            access_nodes < 1 or
            edge_nodes < 1 or
            ues < 1 or
            services < 0
        ):
            return jsonify({
                "success": False,
                "error": "Invalid network configuration."
            }), 400

        # Pass the GUI configuration directly to ns-3.
        command = [
            str(NETWORK_EXECUTABLE),
            f"--accessNodes={access_nodes}",
            f"--edgeNodes={edge_nodes}",
            f"--ues={ues}",
            f"--services={services}"
        ]

        result = subprocess.run(
            command,
            cwd=str(PROJECT_ROOT),
            capture_output=True,
            text=True,
            timeout=120
        )

        output = result.stdout

        # Parse actual ns-3 network information.
        network_data = {
            "ue": None,
            "access_node": None,
            "reachable_edge_nodes": 0,
            "edge_node": None,
            "ue_connection": None,
            "access_edge_path": None,
            "services": None
        }

        access_count = None
        edge_count = None
        ue_count = None
        service_count = None

        for line in output.splitlines():
            line = line.strip()

            if line.startswith("Access Nodes") and ":" in line:
                access_count = int(
                    line.split(":", 1)[1].strip()
                )

            elif line.startswith("Edge Nodes") and ":" in line:
                edge_count = int(
                    line.split(":", 1)[1].strip()
                )

            elif line.startswith("UEs") and ":" in line:
                ue_count = int(
                    line.split(":", 1)[1].strip()
                )

            elif line.startswith("Services") and ":" in line:
                service_count = int(
                    line.split(":", 1)[1].strip()
                )

        network_data["access_node"] = access_count
        network_data["edge_node"] = edge_count
        network_data["ue"] = ue_count
        network_data["services"] = service_count
        network_data["reachable_edge_nodes"] = edge_count or 0

        network_data["ue_connection"] = (
            "VALID"
            if "BASIC NETWORK READY" in output
            else "INVALID"
        )

        network_data["access_edge_path"] = (
            "VALID"
            if edge_count and access_count
            else "INVALID"
        )

        return jsonify({
            "success": result.returncode == 0,
            "output": output,
            "error": result.stderr,
            "network": network_data
        })

    except (TypeError, ValueError):
        return jsonify({
            "success": False,
            "error": "Network configuration must contain valid numbers."
        }), 400

    except subprocess.TimeoutExpired:
        return jsonify({
            "success": False,
            "error": "Network simulation timed out."
        }), 500

    except Exception as error:
        return jsonify({
            "success": False,
            "error": str(error)
        }), 500

# Dynamic Resource State
DYNAMIC_RESOURCE_STATE_EXECUTABLE = (
    PROJECT_ROOT
    / "build"
    / "contrib"
    / "6g-lena"
    / "network-nodes"
    / "examples"
    / "ns3.48-6g-dynamic-resource-state-example-default"
)


@app.route("/api/capabilities/resource-state")
def capability_resource_state():

    executable = DYNAMIC_RESOURCE_STATE_EXECUTABLE

    if not executable.exists():
        return jsonify({
            "success": False,
            "error": (
                "Dynamic resource state executable not found: "
                + str(executable)
            )
        }), 500

    try:

        result = subprocess.run(
            [str(executable)],
            cwd=str(PROJECT_ROOT),
            capture_output=True,
            text=True,
            timeout=120
        )

        output = result.stdout

        import re

        capacity_match = re.search(
            r"Capacity\s*:\s*([0-9.]+)",
            output
        )

        available_consume_match = re.search(
            r"Available after consume\s*:\s*([0-9.]+)",
            output
        )

        available_release_match = re.search(
            r"Available after release\s*:\s*([0-9.]+)",
            output
        )

        utilization_consume_match = re.search(
            r"Utilization after consume\s*:\s*([0-9.]+)",
            output
        )

        utilization_release_match = re.search(
            r"Utilization after release\s*:\s*([0-9.]+)",
            output
        )

        node_match = re.search(
            r"Node\s*:\s*(.+)",
            output
        )

        resource_match = re.search(
            r"Resource\s*:\s*(.+)",
            output
        )

        usable_match = re.search(
            r"Capability usable\s*:\s*(YES|NO)",
            output
        )

        capacity = (
            float(capacity_match.group(1))
            if capacity_match else None
        )

        available_after_consume = (
            float(available_consume_match.group(1))
            if available_consume_match else None
        )

        available_after_release = (
            float(available_release_match.group(1))
            if available_release_match else None
        )

        utilization_after_consume = (
            float(utilization_consume_match.group(1))
            if utilization_consume_match else None
        )

        utilization_after_release = (
            float(utilization_release_match.group(1))
            if utilization_release_match else None
        )

        return jsonify({
            "success": result.returncode == 0,
            "node": node_match.group(1).strip()
                if node_match else None,
            "resource": resource_match.group(1).strip()
                if resource_match else None,
            "capacity": capacity,
            "available_after_consume": available_after_consume,
            "available_after_release": available_after_release,
            "utilization_after_consume": utilization_after_consume,
            "utilization_after_release": utilization_after_release,
            "capability_usable": (
                usable_match.group(1) == "YES"
                if usable_match else None
            ),
            "output": output,
            "error": result.stderr
        })

    except subprocess.TimeoutExpired:

        return jsonify({
            "success": False,
            "error": "Dynamic resource state simulation timed out."
        }), 500

# Service Placement
SERVICE_PLACEMENT_EXECUTABLE = (
    PROJECT_ROOT
    / "build"
    / "contrib"
    / "6g-lena"
    / "network-nodes"
    / "examples"
    / "ns3.48-6g-service-placement-example-default"
)


@app.route("/api/services/placement")
def service_placement():

    executable = SERVICE_PLACEMENT_EXECUTABLE

    if not executable.exists():
        return jsonify({
            "success": False,
            "error": (
                "Service placement executable not found: "
                + str(executable)
            )
        }), 500

    try:

        result = subprocess.run(
            [str(executable)],
            cwd=str(PROJECT_ROOT),
            capture_output=True,
            text=True,
            timeout=120
        )

        output = result.stdout

        import re

        service_match = re.search(
            r"Service\s*:\s*(.+)",
            output
        )

        compute_required_match = re.search(
            r"Required compute\s*:\s*([0-9.]+)",
            output
        )

        communication_required_match = re.search(
            r"Required communication\s*:\s*([0-9.]+)",
            output
        )

        selected_edge_match = re.search(
            r"Selected Edge Node\s*:\s*(.+)",
            output
        )

        ue_match = re.search(
            r"UE\s*:\s*(.+)",
            output
        )

        access_match = re.search(
            r"Access Node\s*:\s*(.+)",
            output
        )

        active_match = re.search(
            r"Service active\s*:\s*(YES|NO)",
            output
        )

        compute_left_match = re.search(
            r"Compute left\s*:\s*([0-9.]+)",
            output
        )

        communication_left_match = re.search(
            r"Communication left\s*:\s*([0-9.]+)",
            output
        )

        candidates = []

        for match in re.finditer(
            r"Candidate:\s*(.+?)\s*\|\s*available compute\s*=\s*([0-9.]+)",
            output
        ):
            candidates.append({
                "node": match.group(1).strip(),
                "available_compute": float(match.group(2))
            })

        return jsonify({
            "success": result.returncode == 0,
            "service": (
                service_match.group(1).strip()
                if service_match else None
            ),
            "required_compute": (
                float(compute_required_match.group(1))
                if compute_required_match else None
            ),
            "required_communication": (
                float(communication_required_match.group(1))
                if communication_required_match else None
            ),
            "selected_edge": (
                selected_edge_match.group(1).strip()
                if selected_edge_match else None
            ),
            "ue": (
                ue_match.group(1).strip()
                if ue_match else None
            ),
            "access_node": (
                access_match.group(1).strip()
                if access_match else None
            ),
            "service_active": (
                active_match.group(1) == "YES"
                if active_match else None
            ),
            "compute_left": (
                float(compute_left_match.group(1))
                if compute_left_match else None
            ),
            "communication_left": (
                float(communication_left_match.group(1))
                if communication_left_match else None
            ),
            "candidates": candidates,
            "output": output,
            "error": result.stderr
        })

    except subprocess.TimeoutExpired:

        return jsonify({
            "success": False,
            "error": "Service placement simulation timed out."
        }), 500




END_TO_END_SESSION_EXECUTABLE = (
    PROJECT_ROOT
    / "build"
    / "contrib"
    / "6g-lena"
    / "network-nodes"
    / "examples"
    / "ns3.48-6g-end-to-end-session-experiment-default"
)


@app.route("/api/services/end-to-end-session")
def end_to_end_session():
    try:
        def get_positive_float(name, default):
            value = request.args.get(name, default)
            try:
                value = float(value)
            except (TypeError, ValueError):
                raise ValueError(f"{name} must be a valid number.")

            if value <= 0:
                raise ValueError(f"{name} must be greater than 0.")

            return value

        communication_capacity = get_positive_float(
            "communicationCapacity", 100.0
        )
        computing_capacity = get_positive_float(
            "computingCapacity", 100.0
        )
        communication_required = get_positive_float(
            "communicationRequired", 30.0
        )
        computing_required = get_positive_float(
            "computingRequired", 40.0
        )

        command = [
            str(END_TO_END_SESSION_EXECUTABLE),
            f"--communicationCapacity={communication_capacity:g}",
            f"--computingCapacity={computing_capacity:g}",
            f"--communicationRequired={communication_required:g}",
            f"--computingRequired={computing_required:g}",
        ]

        result = subprocess.run(
            command,
            capture_output=True,
            text=True,
            cwd=str(PROJECT_ROOT),
            timeout=60,
            check=False,
        )

        output = result.stdout

        if result.returncode != 0:
            return jsonify({
                "success": False,
                "error": result.stderr.strip()
                or "End-to-end session experiment failed.",
                "stdout": output,
            }), 500

        def extract(pattern, default=None):
            match = re.search(pattern, output)
            return match.group(1).strip() if match else default

        data = {
            "parameters": {
                "communication_capacity": communication_capacity,
                "computing_capacity": computing_capacity,
                "communication_required": communication_required,
                "computing_required": computing_required,
            },
            "initial": {
                "communication_capacity": extract(
                    r"Communication capacity\s*:\s*([0-9.]+)"
                ),
                "communication_available": extract(
                    r"Communication available\s*:\s*([0-9.]+)"
                ),
                "computing_capacity": extract(
                    r"Computing capacity\s*:\s*([0-9.]+)"
                ),
                "computing_available": extract(
                    r"Computing available\s*:\s*([0-9.]+)"
                ),
            },
            "requirements": {
                "communication": extract(
                    r"Communication required:\s*([0-9.]+)"
                ),
                "computing": extract(
                    r"Computing required\s*:\s*([0-9.]+)"
                ),
            },
            "activation": {
                "status": extract(
                    r"Service activation:\s*(\w+)"
                ),
                "communication_available": extract(
                    r"Communication available after activation:\s*([0-9.]+)"
                ),
                "computing_available": extract(
                    r"Computing available after activation\s*:\s*([0-9.]+)"
                ),
            },
            "deactivation": {
                "status": extract(
                    r"Service deactivation:\s*(\w+)"
                ),
                "communication_available": extract(
                    r"Communication available after release:\s*([0-9.]+)"
                ),
                "computing_available": extract(
                    r"Computing available after release\s*:\s*([0-9.]+)"
                ),
            },
            "checks": {
                "activation_resource_check": extract(
                    r"Activation resource check\s*:\s*(\w+)"
                ),
                "release_resource_check": extract(
                    r"Release resource check\s*:\s*(\w+)"
                ),
            },
            "experiment_result": (
                "PASSED"
                if "END-TO-END SESSION EXPERIMENT PASSED" in output
                else "FAILED"
            ),
        }

        data["success"] = data["experiment_result"] == "PASSED"
        data["raw_output"] = output

        return jsonify(data)

    except ValueError as exc:
        return jsonify({
            "success": False,
            "error": str(exc),
        }), 400

    except Exception as exc:
        return jsonify({
            "success": False,
            "error": str(exc),
        }), 500


@app.route("/api/resources/stress")
def resource_stress():
    try:
        node_a_capacity = request.args.get("nodeACapacity", "100")
        node_b_capacity = request.args.get("nodeBCapacity", "80")
        request_count = request.args.get("requestCount", "5")
        request_start = request.args.get("requestStart", "20")
        request_step = request.args.get("requestStep", "20")

        node_a_capacity = float(node_a_capacity)
        node_b_capacity = float(node_b_capacity)
        request_count = int(request_count)
        request_start = float(request_start)
        request_step = float(request_step)

        if (
            node_a_capacity <= 0
            or node_b_capacity <= 0
            or request_count <= 0
            or request_start < 0
            or request_step < 0
        ):
            return jsonify({
                "success": False,
                "error": "Invalid Resource Stress parameters."
            }), 400

        command = [
            str(RESOURCE_STRESS_EXECUTABLE),
            f"--nodeACapacity={node_a_capacity:g}",
            f"--nodeBCapacity={node_b_capacity:g}",
            f"--requestCount={request_count}",
            f"--requestStart={request_start:g}",
            f"--requestStep={request_step:g}",
        ]

        result = subprocess.run(
            command,
            capture_output=True,
            text=True,
            cwd=str(PROJECT_ROOT),
            timeout=60,
            check=False,
        )

        output = result.stdout

        if result.returncode != 0:
            return jsonify({
                "success": False,
                "error": result.stderr.strip() or "Resource stress experiment failed.",
                "stdout": output,
            }), 500

        def extract(pattern, default="—"):
            match = re.search(pattern, output)
            return match.group(1).strip() if match else default

        requests = []

        for match in re.finditer(
            r"Request\s+(\d+)\s+\|\s+mode=(STATIC|DYNAMIC)\s+\|"
            r"\s+required=([0-9.]+)\s+\|\s+selected=([^|]+)"
            r"\s+\|\s+result=(SUCCESS|FAILURE)",
            output,
        ):
            requests.append({
                "request": match.group(1),
                "mode": match.group(2),
                "required": match.group(3),
                "selected": match.group(4).strip(),
                "result": match.group(5),
            })

        static = {
            "requests": extract(
                r"STATIC SELECTION\s+Requests\s*:\s*([0-9]+)"
            ),
            "successful": extract(
                r"STATIC SELECTION\s+Requests\s*:\s*[0-9]+\s+Successful\s*:\s*([0-9]+)"
            ),
            "failed": extract(
                r"STATIC SELECTION\s+Requests\s*:\s*[0-9]+\s+Successful\s*:\s*[0-9]+\s+Failed\s*:\s*([0-9]+)"
            ),
            "success_rate": extract(
                r"STATIC SELECTION\s+Requests\s*:[\s\S]*?Success rate\s*:\s*([0-9.]+%)"
            ),
            "final_edgenode_1": extract(
                r"STATIC SELECTION\s+Requests\s*:[\s\S]*?Final EdgeNode-1\s*:\s*([0-9.]+)"
            ),
            "final_edgenode_2": extract(
                r"STATIC SELECTION\s+Requests\s*:[\s\S]*?Final EdgeNode-2\s*:\s*([0-9.]+)"
            ),
        }

        dynamic = {
            "requests": extract(
                r"DYNAMIC CAPABILITY DISCOVERY\s+Requests\s*:\s*([0-9]+)"
            ),
            "successful": extract(
                r"DYNAMIC CAPABILITY DISCOVERY\s+Requests\s*:[\s\S]*?Successful\s*:\s*([0-9]+)"
            ),
            "failed": extract(
                r"DYNAMIC CAPABILITY DISCOVERY\s+Requests\s*:[\s\S]*?Failed\s*:\s*([0-9]+)"
            ),
            "discovery_matches": extract(
                r"Discovery matches\s*:\s*([0-9]+)"
            ),
            "success_rate": extract(
                r"DYNAMIC CAPABILITY DISCOVERY\s+Requests\s*:[\s\S]*?Success rate\s*:\s*([0-9.]+%)"
            ),
            "final_edgenode_1": extract(
                r"DYNAMIC CAPABILITY DISCOVERY\s+Requests\s*:[\s\S]*?Final EdgeNode-1\s*:\s*([0-9.]+)"
            ),
            "final_edgenode_2": extract(
                r"DYNAMIC CAPABILITY DISCOVERY\s+Requests\s*:[\s\S]*?Final EdgeNode-2\s*:\s*([0-9.]+)"
            ),
        }

        improvement = extract(
            r"Dynamic success-rate improvement:\s*([0-9.]+ percentage points)"
        )

        return jsonify({
            "success": "=== Experiment completed ===" in output,
            "requests": requests,
            "summary": {
                "STATIC": static,
                "DYNAMIC": dynamic,
            },
            "improvement": improvement,
            "raw_output": output,
        })

    except Exception as exc:
        return jsonify({
            "success": False,
            "error": str(exc),
        }), 500

@app.route("/api/capabilities/constraint")
def capability_constraint():

    executable = CAPABILITY_CONSTRAINT_EXECUTABLE

    try:
        minimum_resource = float(request.args.get("minimumResource", "80"))
        if minimum_resource <= 0:
            raise ValueError("Minimum resource must be greater than 0")
    except ValueError as exc:
        return jsonify({
            "success": False,
            "error": str(exc),
        }), 400

    if not executable.exists():
        return jsonify({
            "success": False,
            "error": "Capability constraint executable not found: " + str(executable)
        }), 500

    try:
        result = subprocess.run(
            [
                str(executable),
                f"--minimum-resource={minimum_resource:g}"
            ],
            cwd=str(PROJECT_ROOT),
            capture_output=True,
            text=True,
            timeout=120
        )

        if result.returncode != 0:
            return jsonify({
                "success": False,
                "error": result.stderr.strip() or "Capability constraint simulation failed.",
                "output": result.stdout
            }), 500

        lines = [
            line.strip()
            for line in result.stdout.splitlines()
            if line.strip()
        ]

        requests_data = []

        for i, line in enumerate(lines):

            if not line.startswith("Request ") or "|" not in line:
                continue

            request_name, constraint = [
                part.strip()
                for part in line.split("|", 1)
            ]

            discovered = 0
            providers = []
            expected = []
            correctness = "UNKNOWN"

            for following in lines[i + 1:i + 5]:

                if following.startswith("Discovered providers"):
                    try:
                        discovered = int(
                            following.split(":", 1)[1].strip()
                        )
                    except ValueError:
                        pass

                elif following.startswith("Providers"):
                    value = following.split(":", 1)[1].strip()

                    if value:
                        providers = [
                            item.strip()
                            for item in value.split(",")
                        ]

                elif following.startswith("Expected providers"):
                    value = following.split(":", 1)[1].strip()

                    if value:
                        expected = [
                            item.strip()
                            for item in value.split(",")
                        ]

                elif following.startswith("Discovery correctness"):
                    correctness = following.split(":", 1)[1].strip()

            requests_data.append({
                "request": request_name,
                "constraint": constraint,
                "discovered": discovered,
                "providers": providers,
                "expected": expected,
                "correctness": correctness
            })

        summary = {}

        for line in lines:

            if ":" not in line:
                continue

            key, value = line.split(":", 1)
            key = key.strip().lower().replace(" ", "_")
            value = value.strip()

            if key in (
                "discovery_requests",
                "successful_discoveries",
                "total_provider_matches",
                "correct_discoveries"
            ):
                try:
                    summary[key] = int(value)
                except ValueError:
                    pass

        correctness_rate = None

        for line in lines:
            if line.startswith("Correctness rate"):
                try:
                    correctness_rate = float(
                        line.split(":", 1)[1]
                        .replace("%", "")
                        .strip()
                    )
                except ValueError:
                    pass

        return jsonify({
            "success": True,
            "requests": requests_data,
            "correctness_rate": correctness_rate,
            "summary": summary,
            "output": result.stdout
        })

    except subprocess.TimeoutExpired:
        return jsonify({
            "success": False,
            "error": "Capability constraint simulation timed out."
        }), 500

    except Exception as e:
        return jsonify({
            "success": False,
            "error": str(e)
        }), 500


@app.route("/api/capabilities/experiment")
def capability_experiment():

    executable = CAPABILITY_EXPERIMENT_EXECUTABLE

    if not executable.exists():
        return jsonify({
            "success": False,
            "error": "Capability experiment executable not found: " + str(executable)
        }), 500

    try:
        result = subprocess.run(
            [str(executable)],
            cwd=str(PROJECT_ROOT),
            capture_output=True,
            text=True,
            timeout=120
        )

        if result.returncode != 0:
            return jsonify({
                "success": False,
                "error": result.stderr.strip() or "Capability experiment failed.",
                "output": result.stdout
            }), 500

        lines = [
            line.strip()
            for line in result.stdout.splitlines()
            if line.strip()
        ]

        experiment = {
            "workload_profile": None,
            "runtime_updates": None,
            "request_count": None
        }

        requests = {
            "STATIC": [],
            "DYNAMIC": []
        }

        summary = {
            "STATIC": {},
            "DYNAMIC": {}
        }

        comparison = {}

        current_mode = None
        current_summary = None

        for line in lines:

            if line.startswith("Workload profile"):
                experiment["workload_profile"] = line.split(":", 1)[1].strip()

            elif line.startswith("Runtime updates"):
                experiment["runtime_updates"] = line.split(":", 1)[1].strip()

            elif line.startswith("Request count"):
                try:
                    experiment["request_count"] = int(
                        line.split(":", 1)[1].strip()
                    )
                except ValueError:
                    pass

            elif line == "STATIC SELECTION EXPERIMENT":
                current_mode = "STATIC"

            elif line == "DYNAMIC CAPABILITY DISCOVERY EXPERIMENT":
                current_mode = "DYNAMIC"

            elif line.startswith("Request ") and "|" in line and current_mode:
                values = {}

                for part in line.split("|"):
                    part = part.strip()

                    if "=" not in part:
                        continue

                    key, value = part.split("=", 1)
                    values[key.strip()] = value.strip()

                requests[current_mode].append(values)

            elif line == "STATIC SELECTION":
                current_summary = "STATIC"

            elif line == "DYNAMIC CAPABILITY DISCOVERY":
                current_summary = "DYNAMIC"

            elif line.startswith("Success-rate difference"):
                comparison["success_rate_difference"] = line.split(":", 1)[1].strip()

            elif line.startswith("Allocation-ratio diff"):
                comparison["allocation_ratio_difference"] = line.split(":", 1)[1].strip()

            elif current_summary and ":" in line:
                key, value = line.split(":", 1)

                key = (
                    key.strip()
                    .lower()
                    .replace(" ", "_")
                )

                summary[current_summary][key] = value.strip()

        return jsonify({
            "success": True,
            "experiment": experiment,
            "requests": requests,
            "summary": summary,
            "comparison": comparison,
            "output": result.stdout
        })

    except subprocess.TimeoutExpired:
        return jsonify({
            "success": False,
            "error": "Capability experiment timed out."
        }), 500

    except Exception as e:
        return jsonify({
            "success": False,
            "error": str(e)
        }), 500


@app.route("/api/capabilities/freshness")
def capability_freshness():
    executable = CAPABILITY_FRESHNESS_EXECUTABLE

    if not executable.exists():
        return jsonify({
            "success": False,
            "error": "Capability freshness executable not found: " + str(executable)
        }), 500

    try:
        node_a_validity = float(request.args.get("nodeAValidity", 4))
        node_b_validity = float(request.args.get("nodeBValidity", 10))
        minimum_resource = float(request.args.get("minimumResource", 50))
        second_checkpoint = float(
            request.args.get("secondCheckpoint", 6)
        )
        third_checkpoint = float(
            request.args.get("thirdCheckpoint", 11)
        )

        if min(
            node_a_validity,
            node_b_validity,
            minimum_resource,
            second_checkpoint,
            third_checkpoint
        ) < 0:
            return jsonify({
                "success": False,
                "error": "Parameters must be non-negative."
            }), 400

        result = subprocess.run(
            [
                str(executable),
                f"--nodeAValidity={node_a_validity}",
                f"--nodeBValidity={node_b_validity}",
                f"--minimumResource={minimum_resource}",
                f"--secondCheckpoint={second_checkpoint}",
                f"--thirdCheckpoint={third_checkpoint}",
            ],
            cwd=str(PROJECT_ROOT),
            capture_output=True,
            text=True,
            timeout=60,
            check=False,
        )

        output = result.stdout

        checkpoints = {}
        current_time = None

        for line in output.splitlines():
            line = line.strip()

            match = re.match(
                r"Capability state at t=([0-9.]+)s",
                line
            )
            if match:
                current_time = float(match.group(1))
                checkpoints.setdefault(
                    current_time,
                    {
                        "label": f"t={current_time:g}s",
                        "time": current_time,
                        "normal_matches": 0,
                        "fresh_matches": 0,
                        "fresh_providers": []
                    }
                )
                continue

            match = re.match(
                r"Request \d+ \| t=([0-9.]+)s \| requireFresh=(TRUE|FALSE)",
                line
            )
            if match:
                current_time = float(match.group(1))
                checkpoints.setdefault(
                    current_time,
                    {
                        "label": f"t={current_time:g}s",
                        "time": current_time,
                        "normal_matches": 0,
                        "fresh_matches": 0,
                        "fresh_providers": []
                    }
                )

                current_request_fresh = (
                    match.group(2) == "TRUE"
                )
                continue

            if line.startswith("Discovered providers :") and current_time is not None:
                providers_text = line.split(":", 1)[1].strip()

                if providers_text:
                    providers = [
                        x.strip()
                        for x in providers_text.split(",")
                    ]
                else:
                    providers = []

                if current_request_fresh:
                    checkpoints[current_time]["fresh_matches"] = len(providers)
                    checkpoints[current_time]["fresh_providers"] = providers
                else:
                    checkpoints[current_time]["normal_matches"] = len(providers)

        checkpoints = [
            checkpoints[t]
            for t in sorted(checkpoints)
        ]

        return jsonify({
            "success": result.returncode == 0,
            "parameters": {
                "nodeAValidity": node_a_validity,
                "nodeBValidity": node_b_validity,
                "minimumResource": minimum_resource,
                "secondCheckpoint": second_checkpoint,
                "thirdCheckpoint": third_checkpoint,
            },
            "checkpoints": checkpoints,
            "experiment_status": (
                "COMPLETED" if result.returncode == 0 else "FAILED"
            ),
            "raw_output": output,
        })

    except Exception as e:
        return jsonify({
            "success": False,
            "error": str(e)
        }), 500

@app.route("/api/capabilities/update")
def capability_update():

    executable = CAPABILITY_UPDATE_EXECUTABLE

    if not executable.exists():
        return jsonify({
            "success": False,
            "error": "Capability update executable not found: " + str(executable)
        }), 500

    try:
        result = subprocess.run(
            [str(executable)],
            cwd=str(PROJECT_ROOT),
            capture_output=True,
            text=True,
            timeout=120
        )

        if result.returncode != 0:
            return jsonify({
                "success": False,
                "error": result.stderr.strip() or "Capability update simulation failed.",
                "output": result.stdout
            }), 500

        states = []

        for line in result.stdout.splitlines():
            line = line.strip()

            if not line.startswith("time="):
                continue

            values = {}

            providers = []

            for item in line.split(","):
                if "=" in item:
                    key, value = item.split("=", 1)
                    key = key.strip()
                    value = value.strip()

                    if key == "provider":
                        providers.append({
                            "provider": value
                        })
                    elif key == "available_resource":
                        if providers:
                            providers[-1]["available_resource"] = value
                    else:
                        values[key] = value

            if providers:
                values["providers"] = providers

            if values:
                states.append(values)

        return jsonify({
            "success": True,
            "research_question": "Can capability discovery adapt to runtime provider-state changes?",
            "minimum_resource": 60,
            "states": states,
            "output": result.stdout
        })

    except subprocess.TimeoutExpired:
        return jsonify({
            "success": False,
            "error": "Capability update simulation timed out."
        }), 500

    except Exception as e:
        return jsonify({
            "success": False,
            "error": str(e)
        }), 500



@app.route("/api/network/mobility-handover")
def mobility_handover():
    try:
        def get_float(name, default):
            value = request.args.get(name, default)
            return float(value)

        ue_x = get_float("ueX", 25)
        ue_y = get_float("ueY", 0)
        access1_x = get_float("access1X", 0)
        access1_y = get_float("access1Y", 0)
        access2_x = get_float("access2X", 100)
        access2_y = get_float("access2Y", 0)

        executable = PROJECT_ROOT / \
            "build/contrib/6g-lena/network-nodes/examples/" \
            "ns3.48-6g-mobility-handover-example-default"

        result = subprocess.run(
            [
                str(executable),
                f"--ueX={ue_x}",
                f"--ueY={ue_y}",
                f"--access1X={access1_x}",
                f"--access1Y={access1_y}",
                f"--access2X={access2_x}",
                f"--access2Y={access2_y}"
            ],
            cwd=str(PROJECT_ROOT),
            capture_output=True,
            text=True,
            timeout=60
        )

        if result.returncode != 0:
            return jsonify({
                "success": False,
                "error": result.stderr.strip() or
                         "Mobility handover simulation failed.",
                "output": result.stdout
            }), 500

        output = result.stdout

        def extract(pattern, default=None):
            match = re.search(pattern, output)
            return match.group(1).strip() if match else default

        ue_position = extract(
            r"UE position\s*:\s*\(([^)]+)\)"
        )

        distance1 = extract(
            r"Distance to AccessNode-1\s*:\s*([0-9.eE+-]+)"
        )

        distance2 = extract(
            r"Distance to AccessNode-2\s*:\s*([0-9.eE+-]+)"
        )

        handover_required = extract(
            r"Handover required\s*:\s*(YES|NO)"
        )

        serving_node = extract(
            r"New Serving Access Node\s*:\s*(.+)"
        )

        reachable_edge = extract(
            r"Reachable Edge Node\s*:\s*(.+)"
        )

        handover_status = extract(
            r"Handover status\s*:\s*(SUCCESS|FAILED)"
        )

        return jsonify({
            "success": True,
            "ue_position": ue_position,
            "distance_access1": float(distance1),
            "distance_access2": float(distance2),
            "handover_required": handover_required == "YES",
            "serving_access_node": serving_node,
            "reachable_edge_node": reachable_edge,
            "handover_status": handover_status,
            "parameters": {
                "ueX": ue_x,
                "ueY": ue_y,
                "access1X": access1_x,
                "access1Y": access1_y,
                "access2X": access2_x,
                "access2Y": access2_y
            },
            "output": output
        })

    except ValueError:
        return jsonify({
            "success": False,
            "error": "Mobility parameters must be valid numbers."
        }), 400

    except subprocess.TimeoutExpired:
        return jsonify({
            "success": False,
            "error": "Mobility handover simulation timed out."
        }), 500

    except Exception as e:
        return jsonify({
            "success": False,
            "error": str(e)
        }), 500



@app.route("/api/network/mobility-handover-experiment")
def mobility_handover_experiment():
    try:
        access1_capacity = float(request.args.get("access1CommunicationCapacity", 100))
        access2_capacity = float(request.args.get("access2CommunicationCapacity", 100))
        service_comm = float(request.args.get("serviceCommunicationRequirement", 20))
        service_compute = float(request.args.get("serviceComputeRequirement", 20))

        if min(access1_capacity, access2_capacity, service_comm, service_compute) < 0:
            return jsonify({"error": "Parameters must be non-negative."}), 400

        executable = (
            PROJECT_ROOT
            / "build/contrib/6g-lena/network-nodes/examples/"
            "ns3.48-6g-mobility-handover-experiment-default"
        )

        result = subprocess.run(
            [
                str(executable),
                f"--access1CommunicationCapacity={access1_capacity}",
                f"--access2CommunicationCapacity={access2_capacity}",
                f"--serviceCommunicationRequirement={service_comm}",
                f"--serviceComputeRequirement={service_compute}",
            ],
            cwd=str(PROJECT_ROOT),
            capture_output=True,
            text=True,
            timeout=60,
            check=False,
        )

        output = result.stdout

        def get_status(pattern):
            match = re.search(pattern, output)
            return match.group(1) if match else "—"

        return jsonify({
            "success": result.returncode == 0,
            "parameters": {
                "access1CommunicationCapacity": access1_capacity,
                "access2CommunicationCapacity": access2_capacity,
                "serviceCommunicationRequirement": service_comm,
                "serviceComputeRequirement": service_compute,
            },
            "initial_activation": get_status(
                r"Initial connection:[\s\S]*?Session activation:\s*(SUCCESS|FAILED)"
            ),
            "old_session_release": get_status(
                r"Old session release: (PASS|FAIL)"
            ),
            "new_access_connection": get_status(
                r"New AccessNode-2 connection: (PASS|FAIL)"
            ),
            "new_session_activation": get_status(
                r"New session activation: (PASS|FAIL)"
            ),
            "old_resource_restored": get_status(
                r"Old communication resource restored: (PASS|FAIL)"
            ),
            "new_resource_reserved": get_status(
                r"New communication resource reserved: (PASS|FAIL)"
            ),
            "experiment_status": (
                "PASSED"
                if result.returncode == 0
                else "FAILED"
            ),
            "raw_output": output,
        })

    except Exception as e:
        return jsonify({"error": str(e)}), 500


if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000, debug=True)
