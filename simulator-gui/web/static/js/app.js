let scalabilityChart = null;

// ============================================================
// Configurable Capability-Based Service Placement
// ============================================================

async function runServicePlacementExperiment() {

    const repetitions =
        document.getElementById("service-placement-repetitions").value;

    const panel =
        document.getElementById("service-placement-result-panel");

    const output =
        document.getElementById("service-placement-result");

    const outputStatus =
        document.getElementById("service-placement-result-status");

    panel.style.display = "block";

    outputStatus.textContent = "RUNNING";
    outputStatus.className = "experiment-status";

    output.textContent =
        "Running configured ns-3 experiment...";

    panel.scrollIntoView({
        behavior: "smooth",
        block: "nearest"
    });

    try {

        const response = await fetch(
            "/api/experiment/service-placement",
            {
                method: "POST",
                headers: {
                    "Content-Type": "application/json"
                },
                body: JSON.stringify({
                    repetitions: repetitions
                })
            }
        );

        const data = await response.json();

        if (data.success) {

            outputStatus.textContent = "PASSED";
            outputStatus.className =
                "experiment-status success";

            output.textContent =
                data.output ||
                "Simulation completed successfully.";

            // ====================================================
            // Update Services Panel — Dynamic Service Placement
            // ====================================================

            const simulationOutput = data.output || "";

            const placementRows = [
                ...simulationOutput.matchAll(
                    /request=(\d+),selected_node=([^,]+),status=(SUCCESS|FAILED)/g
                )
            ].map(match => ({
                request: Number(match[1]),
                node: match[2],
                status: match[3]
            }));

            const successfulPlacements =
                placementRows.filter(row => row.status === "SUCCESS");

            const totalRequests = placementRows.length;
            const successfulCount = successfulPlacements.length;

            const placementsElement =
                document.getElementById("services-panel-placement-result");

            const summaryElement =
                document.getElementById("service-placement-summary");

            const statusElement =
                document.getElementById("service-placement-status");

            const placementStateElement =
                document.getElementById("service-placement-state");

            const serviceNameElement =
                document.getElementById("service-placement-name");

            const sessionCountElement =
                document.getElementById("service-session-count");

            const configuredCountElement =
                document.getElementById("service-configured-count");

            if (placementRows.length > 0) {

                if (serviceNameElement) {
                    serviceNameElement.textContent = "Service-1";
                }

                if (configuredCountElement) {
                    configuredCountElement.textContent = "1";
                }

                if (sessionCountElement) {
                    sessionCountElement.textContent =
                        String(successfulCount);
                }

                if (placementsElement) {
                    placementsElement.innerHTML =
                        placementRows.map(row => {
                            if (row.status === "SUCCESS") {
                                return (
                                    "Request " +
                                    row.request +
                                    " → " +
                                    row.node
                                );
                            }

                            return (
                                "Request " +
                                row.request +
                                " → No suitable node"
                            );
                        }).join("<br>");
                }

                if (summaryElement) {
                    summaryElement.textContent =
                        successfulCount +
                        " / " +
                        totalRequests +
                        " requests placed successfully";
                }

                if (statusElement) {
                    statusElement.textContent =
                        successfulCount +
                        " / " +
                        totalRequests +
                        " PLACED";
                }

                if (placementStateElement) {
                    placementStateElement.textContent = "UPDATED";
                }

            } else {

                if (placementsElement) {
                    placementsElement.textContent =
                        "No placement result available";
                }

                if (summaryElement) {
                    summaryElement.textContent =
                        "Simulation returned no placement requests";
                }

                if (statusElement) {
                    statusElement.textContent = "NO RESULT";
                }

                if (placementStateElement) {
                    placementStateElement.textContent = "—";
                }
            }

        } else {

            outputStatus.textContent = "FAILED";
            outputStatus.className =
                "experiment-status failed";

            output.textContent =
                data.error ||
                data.output ||
                "Simulation failed.";
        }

    } catch (error) {

        outputStatus.textContent = "ERROR";
        outputStatus.className =
            "experiment-status failed";

        output.textContent =
            "Unable to execute simulation.\n\n" +
            error.message;
    }
}

// ============================================================
// Framework Example Runner
// ============================================================

// ============================================================
// Configurable Communication + Computing Load Experiment
// ============================================================

async function runCommunicationComputingLoad() {

    const communicationCapacity =
        document.getElementById("cc-load-communication-capacity").value;

    const computingCapacity =
        document.getElementById("cc-load-computing-capacity").value;

    const communicationPerSession =
        document.getElementById("cc-load-communication-session").value;

    const computingPerSession =
        document.getElementById("cc-load-computing-session").value;

    const panel =
        document.getElementById("cc-load-result-panel");

    const output =
        document.getElementById("cc-load-result");

    const outputStatus =
        document.getElementById("cc-load-result-status");

    panel.style.display = "block";

    outputStatus.textContent = "RUNNING";
    outputStatus.className = "experiment-status";

    output.textContent =
        "Running configured ns-3 experiment...";

    panel.scrollIntoView({
        behavior: "smooth",
        block: "nearest"
    });

    try {

        const response = await fetch(
            "/api/experiment/communication-computing-load",
            {
                method: "POST",
                headers: {
                    "Content-Type": "application/json"
                },
                body: JSON.stringify({
                    communicationCapacity:
                        communicationCapacity,

                    computingCapacity:
                        computingCapacity,

                    communicationPerSession:
                        communicationPerSession,

                    computingPerSession:
                        computingPerSession
                })
            }
        );

        const data = await response.json();

        if (data.success) {

            outputStatus.textContent = "PASSED";
            outputStatus.className =
                "experiment-status success";

            output.textContent =
                data.output ||
                "Simulation completed successfully.";

        } else {

            outputStatus.textContent = "FAILED";
            outputStatus.className =
                "experiment-status failed";

            output.textContent =
                data.error ||
                data.output ||
                "Simulation failed.";
        }

    } catch (error) {

        outputStatus.textContent = "ERROR";
        outputStatus.className =
            "experiment-status failed";

        output.textContent =
            "Unable to execute simulation.\n\n" +
            error.message;
    }
}

async function runEndToEndSessionExperiment() {

    const panel =
        document.getElementById("end-to-end-session-result-panel");

    const output =
        document.getElementById("end-to-end-session-result");

    const outputStatus =
        document.getElementById("end-to-end-session-result-status");

    const communicationCapacity =
        document.getElementById("e2e-communication-capacity").value;

    const computingCapacity =
        document.getElementById("e2e-computing-capacity").value;

    const communicationRequired =
        document.getElementById("e2e-communication-required").value;

    const computingRequired =
        document.getElementById("e2e-computing-required").value;

    panel.style.display = "block";

    outputStatus.textContent = "RUNNING";
    outputStatus.className = "experiment-status";

    output.textContent =
        "Running end-to-end service session experiment...";

    panel.scrollIntoView({
        behavior: "smooth",
        block: "nearest"
    });

    const params = new URLSearchParams({
        communicationCapacity,
        computingCapacity,
        communicationRequired,
        computingRequired
    });

    try {

        const response = await fetch(
            `/api/services/end-to-end-session?${params.toString()}`
        );

        const data = await response.json();

        if (data.success) {

            outputStatus.textContent = "PASSED";
            outputStatus.className =
                "experiment-status success";

            output.textContent =
                data.raw_output ||
                "Experiment completed successfully.";

        } else {

            outputStatus.textContent = "FAILED";
            outputStatus.className =
                "experiment-status failed";

            output.textContent =
                data.error ||
                data.raw_output ||
                "Experiment failed.";
        }

    } catch (error) {

        outputStatus.textContent = "ERROR";
        outputStatus.className =
            "experiment-status failed";

        output.textContent =
            "Unable to execute experiment.\n\n" +
            error.message;
    }
}

async function runFrameworkExample(name, title, outputId) {

    const panel = document.getElementById("framework-example-output-panel");
    const output = document.getElementById("framework-example-output");
    const outputTitle = document.getElementById("framework-output-title");
    const outputStatus = document.getElementById("framework-output-status");

    if (!panel || !output || !outputTitle || !outputStatus) {
        console.error("Framework output panel not found.");
        return;
    }

    panel.style.display = "block";

    outputTitle.textContent = title;
    outputStatus.textContent = "RUNNING";
    outputStatus.className = "experiment-status";

    output.textContent = "Starting ns-3 simulation...";

    panel.scrollIntoView({
        behavior: "smooth",
        block: "nearest"
    });

    try {

        const response = await fetch(`/api/framework/${name}`);
        const data = await response.json();

        if (data.success) {

            outputStatus.textContent = "PASSED";
            outputStatus.className = "experiment-status success";

            output.textContent =
                data.output ||
                "Simulation completed successfully.";

        } else {

            outputStatus.textContent = "FAILED";
            outputStatus.className = "experiment-status failed";

            output.textContent =
                data.error ||
                data.output ||
                "Simulation failed.";

        }

    } catch (error) {

        outputStatus.textContent = "ERROR";
        outputStatus.className = "experiment-status failed";

        output.textContent =
            "Unable to execute simulation.\n\n" +
            error.message;
    }
}


async function runCapabilityScalability() {

    const button =
        document.getElementById("experiments-scalability-button");

    const resultsPanel =
        document.getElementById("simulation-results");

    const statusBox =
        document.getElementById("results-status");

    const tableBody =
        document.getElementById("results-table-body");

    const observation =
        document.getElementById("research-observation");

    if (button) {
        button.disabled = true;
        button.textContent = "Running Simulation...";
    }

    if (resultsPanel) {
        resultsPanel.classList.add("active");
    }

    if (statusBox) {
        statusBox.innerHTML =
            '<span class="status-dot"></span>' +
            '<span>Running ns-3 simulation...</span>';
    }

    if (tableBody) {
        tableBody.innerHTML =
            '<tr><td colspan="4" style="text-align:center;">' +
            'Running simulation. Please wait...' +
            '</td></tr>';
    }

    if (observation) {
        observation.textContent =
            "Processing measured simulation data...";
    }

    try {

        const response = await fetch(
            "/api/experiment/capability-scalability"
        );

        const data = await response.json();

        if (!data.success) {
            throw new Error(
                data.error || "Simulation failed."
            );
        }

        sessionStorage.setItem(
            "iiitdmk_latest_experiment",
            "Capability Scalability"
        );

        sessionStorage.setItem(
            "iiitdmk_latest_status",
            "COMPLETED"
        );

        const lines = data.output
            .split("\n")
            .map(function(line) {
                return line.trim();
            })
            .filter(function(line) {
                return line.length > 0;
            });

        const headerIndex = lines.findIndex(
            function(line) {
                return line.indexOf(
                    "provider_count,repetitions"
                ) === 0;
            }
        );

        if (headerIndex === -1) {
            throw new Error(
                "Simulation result data could not be parsed."
            );
        }

        const resultRows = [];

        for (
            let i = headerIndex + 1;
            i < lines.length;
            i++
        ) {

            const line = lines[i];

            if (
                line === "Experiment completed." ||
                line.indexOf(",") === -1
            ) {
                continue;
            }

            const values = line.split(",");

            if (values.length !== 5) {
                continue;
            }

            resultRows.push({
                providers: Number(values[0]),
                repetitions: Number(values[1]),
                totalElapsed: Number(values[2]),
                averageDiscovery: Number(values[3]),
                totalMatches: Number(values[4])
            });
        }

        if (resultRows.length === 0) {
            throw new Error(
                "No simulation results were returned."
            );
        }

        /*
         * SUMMARY
         */

        const providerValues =
            resultRows.map(function(row) {
                return row.providers;
            });

        const minProviders =
            Math.min.apply(null, providerValues);

        const maxProviders =
            Math.max.apply(null, providerValues);

        document.getElementById(
            "providers-tested"
        ).textContent =
            minProviders.toLocaleString() +
            " → " +
            maxProviders.toLocaleString();

        document.getElementById(
            "repetitions"
        ).textContent =
            resultRows[0].repetitions.toLocaleString();

        /*
         * RESULTS TABLE
         */

        tableBody.innerHTML = "";

        resultRows.forEach(function(row) {

            const tr = document.createElement("tr");

            tr.innerHTML =
                "<td><strong>" +
                row.providers.toLocaleString() +
                "</strong></td>" +

                "<td>" +
                row.averageDiscovery.toLocaleString(
                    undefined,
                    {
                        minimumFractionDigits: 3,
                        maximumFractionDigits: 3
                    }
                ) +
                " µs</td>" +

                "<td>" +
                row.totalElapsed.toLocaleString(
                    undefined,
                    {
                        minimumFractionDigits: 3,
                        maximumFractionDigits: 3
                    }
                ) +
                " µs</td>" +

                "<td>" +
                row.totalMatches.toLocaleString() +
                "</td>";

            tableBody.appendChild(tr);
        });

        /*
         * CHART
         */

        const canvas =
            document.getElementById("scalability-chart");

        if (
            canvas &&
            typeof Chart !== "undefined"
        ) {

            if (scalabilityChart) {
                scalabilityChart.destroy();
            }

            scalabilityChart = new Chart(
                canvas,
                {
                    type: "line",

                    data: {
                        labels: resultRows.map(
                            function(row) {
                                return row.providers;
                            }
                        ),

                        datasets: [{
                            label:
                                "Average Discovery Time (µs)",

                            data: resultRows.map(
                                function(row) {
                                    return row.averageDiscovery;
                                }
                            ),

                            borderWidth: 3,
                            pointRadius: 5,
                            pointHoverRadius: 7,
                            tension: 0.25,
                            fill: false
                        }]
                    },

                    options: {
                        responsive: true,
                        maintainAspectRatio: false,

                        interaction: {
                            intersect: false,
                            mode: "index"
                        },

                        plugins: {
                            legend: {
                                display: true
                            },

                            tooltip: {
                                callbacks: {
                                    label:
                                        function(context) {
                                            return (
                                                " Discovery: " +
                                                context.parsed.y.toFixed(3) +
                                                " µs"
                                            );
                                        }
                                }
                            }
                        },

                        scales: {
                            x: {
                                title: {
                                    display: true,
                                    text:
                                        "Number of Providers"
                                }
                            },

                            y: {
                                beginAtZero: true,

                                title: {
                                    display: true,
                                    text:
                                        "Average Discovery Time (µs)"
                                }
                            }
                        }
                    }
                }
            );
        }

        /*
         * DATA-DERIVED RESEARCH OBSERVATION
         */

        if (observation) {

            const first = resultRows[0];
            const last =
                resultRows[resultRows.length - 1];

            const growthFactor =
                last.averageDiscovery /
                first.averageDiscovery;

            const percentageIncrease =
                (
                    (last.averageDiscovery -
                        first.averageDiscovery) /
                    first.averageDiscovery
                ) * 100;

            observation.innerHTML =
                "Across the measured provider range, average discovery " +
                "time increased from <strong>" +
                first.averageDiscovery.toFixed(3) +
                " µs</strong> at <strong>" +
                first.providers.toLocaleString() +
                " providers</strong> to <strong>" +
                last.averageDiscovery.toFixed(3) +
                " µs</strong> at <strong>" +
                last.providers.toLocaleString() +
                " providers</strong>. " +

                "This corresponds to a measured increase of " +
                "<strong>" +
                percentageIncrease.toFixed(1) +
                "%</strong>, or approximately " +
                "<strong>" +
                growthFactor.toFixed(2) +
                "×</strong> across the tested range. " +

                "The observation is calculated directly from the " +
                "current ns-3 simulation output."
                ;
        }

        /*
         * SUCCESS
         */

        if (statusBox) {
            statusBox.innerHTML =
                '<span class="status-dot"></span>' +
                '<span>Simulation completed successfully</span>';
        }

    } catch (error) {

        if (statusBox) {
            statusBox.innerHTML =
                '<span class="status-dot"></span>' +
                '<span>Simulation failed: ' +
                error.message +
                '</span>';
        }

        if (tableBody) {
            tableBody.innerHTML =
                '<tr>' +
                '<td colspan="4" style="text-align:center;">' +
                error.message +
                '</td>' +
                '</tr>';
        }

        if (observation) {
            observation.textContent =
                "No research observation available because " +
                "the simulation did not complete successfully.";
        }

    } finally {

        if (button) {
            button.disabled = false;
            button.textContent = "Run Experiment →";
        }
    }
}


function loadLatestResultsSummary() {

    const experiment =
        sessionStorage.getItem("iiitdmk_latest_experiment");

    const status =
        sessionStorage.getItem("iiitdmk_latest_status");

    const experimentElement =
        document.getElementById("results-latest-experiment");

    const statusElement =
        document.getElementById("results-latest-status");

    if (experimentElement) {
        experimentElement.textContent =
            experiment || "—";
    }

    if (statusElement) {
        statusElement.textContent =
            status || "READY";
    }
}


function closeResults() {

    const resultsPanel =
        document.getElementById("simulation-results");

    if (resultsPanel) {
        resultsPanel.classList.remove("active");
    }
}



function runCapabilityDiscovery() {

    const panel = document.getElementById("discovery-results");

    if (panel) {
        panel.classList.add("active");
    }

    const resultContent = document.getElementById("discovery-result-content");

    if (resultContent) {
        resultContent.style.display = "none";
    }

    const status = document.getElementById("discovery-status");

    if (status) {
        status.innerHTML = `
            <span class="status-dot"></span>
            <span>Configure the experiment and run the simulation.</span>
        `;
    }
}


async function executeDiscoverySimulation() {

    const button = document.getElementById("discovery-run-button");

    const requests =
        document.getElementById("discovery-request-input").value;

    const profile =
        document.getElementById("discovery-profile-input").value;

    const updates =
        document.getElementById("discovery-updates-input").value;

    const status =
        document.getElementById("discovery-status");

    const resultContent =
        document.getElementById("discovery-result-content");


    if (!requests || Number(requests) < 1) {

        status.innerHTML = `
            <span class="status-dot"></span>
            <span>Please enter a valid request count.</span>
        `;

        return;
    }


    button.disabled = true;

    button.textContent = "⏳ RUNNING SIMULATION...";


    status.innerHTML = `
        <span class="status-dot"></span>
        <span>Running real ns-3 simulation...</span>
    `;


    resultContent.style.display = "none";


    try {

        const response = await fetch(
            `/api/experiment/capability-discovery?requests=${encodeURIComponent(requests)}&profile=${encodeURIComponent(profile)}&updates=${encodeURIComponent(updates)}`
        );


        const data = await response.json();


        if (!data.success) {

            throw new Error(
                data.error || "Simulation failed."
            );

        }

        sessionStorage.setItem(
            "iiitdmk_latest_experiment",
            "Dynamic Capability Discovery"
        );

        sessionStorage.setItem(
            "iiitdmk_latest_status",
            "COMPLETED"
        );


        const lines =
            data.output
                .split("\n")
                .map(line => line.trim());


        function getNumber(label, start = 0, end = lines.length) {

            for (let i = start; i < end; i++) {

                if (lines[i].startsWith(label)) {

                    const match =
                        lines[i].match(/:\s*(-?\d+(?:\.\d+)?)/);

                    if (match) {
                        return parseFloat(match[1]);
                    }
                }
            }

            return 0;
        }


        const staticStart =
            lines.indexOf("STATIC SELECTION");

        const dynamicStart =
            lines.indexOf("DYNAMIC CAPABILITY DISCOVERY");

        const comparisonStart =
            lines.indexOf("COMPARISON");


        const staticRequests =
            getNumber("Requests", staticStart + 1, dynamicStart);

        const staticSuccessRate =
            getNumber("Success rate", staticStart + 1, dynamicStart);

        const staticFailureRate =
            getNumber("Failure rate", staticStart + 1, dynamicStart);

        const staticAllocationRatio =
            getNumber("Allocation ratio", staticStart + 1, dynamicStart);


        const dynamicRequests =
            getNumber("Requests", dynamicStart + 1, comparisonStart);

        const dynamicSuccessRate =
            getNumber("Success rate", dynamicStart + 1, comparisonStart);

        const dynamicFailureRate =
            getNumber("Failure rate", dynamicStart + 1, comparisonStart);

        const dynamicAllocationRatio =
            getNumber("Allocation ratio", dynamicStart + 1, comparisonStart);

        const discoveryCandidates =
            getNumber("Discovery candidates", dynamicStart + 1, comparisonStart);


        const successDifference =
            getNumber("Success-rate difference", comparisonStart + 1);

        const allocationDifference =
            getNumber("Allocation-ratio diff", comparisonStart + 1);


        document.getElementById("discovery-requests").textContent =
            dynamicRequests;

        document.getElementById("static-success-rate").textContent =
            staticSuccessRate.toFixed(2) + "%";

        document.getElementById("dynamic-success-rate").textContent =
            dynamicSuccessRate.toFixed(2) + "%";

        document.getElementById("discovery-candidates").textContent =
            discoveryCandidates;


        const tbody =
            document.getElementById("discovery-table-body");

        tbody.innerHTML = `

            <tr>
                <td>Success Rate</td>
                <td>${staticSuccessRate.toFixed(2)}%</td>
                <td>${dynamicSuccessRate.toFixed(2)}%</td>
                <td>+${successDifference.toFixed(2)} pp</td>
            </tr>

            <tr>
                <td>Failure Rate</td>
                <td>${staticFailureRate.toFixed(2)}%</td>
                <td>${dynamicFailureRate.toFixed(2)}%</td>
                <td>${(dynamicFailureRate - staticFailureRate).toFixed(2)} pp</td>
            </tr>

            <tr>
                <td>Allocation Ratio</td>
                <td>${staticAllocationRatio.toFixed(2)}%</td>
                <td>${dynamicAllocationRatio.toFixed(2)}%</td>
                <td>+${allocationDifference.toFixed(2)} pp</td>
            </tr>

        `;


        if (window.discoveryChart) {
            window.discoveryChart.destroy();
        }


        const ctx =
            document.getElementById("discovery-chart")
                .getContext("2d");


        window.discoveryChart =
            new Chart(ctx, {

                type: "bar",

                data: {

                    labels: [
                        "Success Rate",
                        "Allocation Ratio"
                    ],

                    datasets: [

                        {
                            label: "Static",
                            data: [
                                staticSuccessRate,
                                staticAllocationRatio
                            ]
                        },

                        {
                            label: "Dynamic",
                            data: [
                                dynamicSuccessRate,
                                dynamicAllocationRatio
                            ]
                        }

                    ]

                },

                options: {

                    responsive: true,

                    maintainAspectRatio: false,

                    scales: {

                        y: {
                            beginAtZero: true,
                            max: 100,
                            ticks: {
                                callback: value => value + "%"
                            }
                        }

                    },

                    plugins: {

                        legend: {
                            labels: {
                                color: "#ffffff"
                            }
                        }

                    }

                }

            });


        document.getElementById("discovery-observation").innerHTML = `

            <strong>Measured observation:</strong>
            The dynamic capability-aware selection recorded
            <strong>${dynamicSuccessRate.toFixed(2)}%</strong>
            success compared with
            <strong>${staticSuccessRate.toFixed(2)}%</strong>
            for static selection in this workload.
            The measured success-rate difference was
            <strong>${successDifference.toFixed(2)} percentage points</strong>,
            while the allocation-ratio difference was
            <strong>${allocationDifference.toFixed(2)} percentage points</strong>.
            These values are generated directly from the completed ns-3 run.

        `;


        resultContent.style.display = "block";


        status.innerHTML = `
            <span class="status-dot"></span>
            <span>Simulation completed successfully.</span>
        `;


    } catch (error) {

        status.innerHTML = `
            <span class="status-dot"></span>
            <span>Simulation failed: ${error.message}</span>
        `;

    }


    button.disabled = false;

    button.textContent = "▶ RUN SIMULATION";

}


function closeDiscoveryResults() {

    const panel =
        document.getElementById("discovery-results");

    if (panel) {
        panel.classList.remove("active");
    }

}


/* =========================================================
   NETWORK WORKSPACE
   ========================================================= */

async function buildNetwork() {

    const access = Number(
        document.getElementById("network-access-input").value
    );

    const edge = Number(
        document.getElementById("network-edge-input").value
    );

    const ue = Number(
        document.getElementById("network-ue-input").value
    );

    const services = Number(
        document.getElementById("network-service-input").value
    );

    const status =
        document.getElementById("network-status");

    const button =
        document.getElementById("network-build-button");

    if (
        !Number.isInteger(access) || access < 1 ||
        !Number.isInteger(edge) || edge < 1 ||
        !Number.isInteger(ue) || ue < 1 ||
        !Number.isInteger(services) || services < 0
    ) {
        status.textContent = "INVALID CONFIGURATION";
        status.style.color = "#ff7d9b";
        return;
    }

    button.disabled = true;
    button.textContent = "⏳ RUNNING NS-3...";

    status.textContent = "RUNNING SIMULATION";
    status.style.color = "#7df9ff";

    try {

        const response = await fetch(
            "/api/network/build",
            {
                method: "POST",
                headers: {
                    "Content-Type": "application/json"
                },
                body: JSON.stringify({
                    access_nodes: access,
                    edge_nodes: edge,
                    ues: ue,
                    services: services
                })
            }
        );

        const data = await response.json();

        if (!data.success) {
            throw new Error(
                data.error || "Network simulation failed."
            );
        }

        const network = data.network || {};

        /*
         * Use ACTUAL values returned by ns-3.
         */

        const actualUE =
            network.ue || "NOT REPORTED";

        const actualAccess =
            network.access_node || "NOT REPORTED";

        const actualEdge =
            network.edge_node || "NOT REPORTED";

        const reachableEdges =
            Number(network.reachable_edge_nodes || 0);

        const ueConnection =
            network.ue_connection || "UNKNOWN";

        const accessEdgePath =
            network.access_edge_path || "UNKNOWN";

        /*
         * Display the actual simulated topology.
         */

        document.getElementById(
            "network-access-count"
        ).textContent =
            actualAccess;

        document.getElementById(
            "network-edge-count"
        ).textContent =
            reachableEdges;

        document.getElementById(
            "network-ue-count"
        ).textContent =
            actualUE;

        document.getElementById(
            "network-service-count"
        ).textContent =
            services;

        /*
         * Build topology from actual ns-3 entities.
         */

        const edgeContainer =
            document.getElementById(
                "edge-topology-nodes"
            );

        const accessContainer =
            document.getElementById(
                "access-topology-nodes"
            );

        const ueContainer =
            document.getElementById(
                "ue-topology-nodes"
            );

        edgeContainer.innerHTML = "";
        accessContainer.innerHTML = "";
        ueContainer.innerHTML = "";

        if (actualEdge !== "NOT REPORTED") {

            edgeContainer.innerHTML = `
                <div class="topology-node">
                    <strong>${actualEdge}</strong>
                    <small>COMPUTING NODE</small>
                </div>
            `;

        } else {

            edgeContainer.innerHTML = `
                <div class="topology-node">
                    <strong>NO EDGE DATA</strong>
                    <small>NS-3 OUTPUT</small>
                </div>
            `;

        }

        if (actualAccess !== "NOT REPORTED") {

            accessContainer.innerHTML = `
                <div class="topology-node">
                    <strong>${actualAccess}</strong>
                    <small>6G ACCESS NODE</small>
                </div>
            `;

        } else {

            accessContainer.innerHTML = `
                <div class="topology-node">
                    <strong>NO ACCESS DATA</strong>
                    <small>NS-3 OUTPUT</small>
                </div>
            `;

        }

        if (actualUE !== "NOT REPORTED") {

            ueContainer.innerHTML = `
                <div class="topology-node">
                    <strong>${actualUE}</strong>
                    <small>USER EQUIPMENT</small>
                </div>
            `;

        } else {

            ueContainer.innerHTML = `
                <div class="topology-node">
                    <strong>NO UE DATA</strong>
                    <small>NS-3 OUTPUT</small>
                </div>
            `;

        }

        /*
         * Show real connectivity status.
         */

        const connectionValid =
            ueConnection.toUpperCase() === "VALID";

        const pathValid =
            accessEdgePath.toUpperCase() === "VALID";

        if (connectionValid && pathValid) {

            status.textContent =
                "NETWORK READY • CONNECTION VALID";

            status.style.color =
                "#70ffb0";

        } else {

            status.textContent =
                "NETWORK BUILT • CHECK CONNECTIVITY";

            status.style.color =
                "#ffd166";
        }

        console.log(
            "Actual ns-3 network:",
            network
        );

    } catch (error) {

        status.textContent =
            "SIMULATION FAILED";

        status.style.color =
            "#ff7d9b";

        console.error(
            "Network simulation error:",
            error
        );

    }

    button.disabled = false;
    button.textContent = "▶ BUILD NETWORK";
}

/* =========================================================
   SIDEBAR NAVIGATION
   ========================================================= */



async function runResourceStressExperiment() {

    const button = document.getElementById(
        "experiments-resource-stress-button"
    );

    const nodeACapacity = Number(
        document.getElementById("resource-node-a-capacity").value
    );

    const nodeBCapacity = Number(
        document.getElementById("resource-node-b-capacity").value
    );

    const requestCount = Number(
        document.getElementById("resource-request-count").value
    );

    const requestStart = Number(
        document.getElementById("resource-request-start").value
    );

    const requestStep = Number(
        document.getElementById("resource-request-step").value
    );

    if (
        nodeACapacity <= 0 ||
        nodeBCapacity <= 0 ||
        requestCount <= 0 ||
        requestStart < 0 ||
        requestStep < 0
    ) {
        alert("Please enter valid simulation parameters.");
        return;
    }

    if (button) {
        button.disabled = true;
        button.textContent = "⏳ RUNNING...";
    }

    try {

        const params = new URLSearchParams({
            nodeACapacity: nodeACapacity,
            nodeBCapacity: nodeBCapacity,
            requestCount: requestCount,
            requestStart: requestStart,
            requestStep: requestStep
        });

        const response = await fetch(
            "/api/resources/stress?" + params.toString()
        );

        const data = await response.json();

        if (!response.ok || !data.success) {
            throw new Error(
                data.error || "Resource stress experiment failed."
            );
        }

        document.getElementById(
            "resource-static-success"
        ).textContent =
            data.summary.STATIC.success_rate;

        document.getElementById(
            "resource-dynamic-success"
        ).textContent =
            data.summary.DYNAMIC.success_rate;

        document.getElementById(
            "resource-dynamic-improvement"
        ).textContent =
            data.improvement;

        const requestContainer = document.getElementById(
            "resource-stress-request-results"
        );

        requestContainer.innerHTML = data.requests.map(
            item => `
                <div class="resource-request-row">
                    <span>
                        ${item.mode} · Request ${item.request}
                    </span>
                    <span>
                        Required: ${item.required}
                        · ${item.selected}
                        · ${item.result}
                    </span>
                </div>
            `
        ).join("");

        document.getElementById(
            "resource-stress-results"
        ).style.display = "block";

        sessionStorage.setItem(
            "iiitdmk_latest_experiment",
            "Resource Stress"
        );

        sessionStorage.setItem(
            "iiitdmk_latest_status",
            "COMPLETED"
        );

    } catch (error) {

        console.error(
            "Resource stress experiment error:",
            error
        );

        alert(
            "SIMULATION FAILED — " +
            error.message
        );

    } finally {

        if (button) {
            button.disabled = false;
            button.textContent = "Run Experiment →";
        }
    }
}
async function runEndToEndSession() {

    const button = document.getElementById("end-to-end-session-button");
    const status = document.getElementById("end-to-end-session-status");
    const results = document.getElementById("end-to-end-session-results");

    if (button) {
        button.disabled = true;
        button.textContent = "⏳ RUNNING...";
    }

    if (status) {
        status.textContent = "RUNNING REAL NS-3 END-TO-END SESSION...";
    }

    if (results) {
        results.style.display = "none";
    }

    try {

        const response = await fetch("/api/services/end-to-end-session");
        const data = await response.json();

        if (!response.ok || !data.success) {
            throw new Error(data.error || "End-to-end session experiment failed.");
        }

        const initial = data.initial || {};
        const requirements = data.requirements || {};
        const activation = data.activation || {};
        const deactivation = data.deactivation || {};
        const checks = data.checks || {};

        document.getElementById("e2e-required-communication").textContent =
            requirements.communication ?? "—";

        document.getElementById("e2e-required-computing").textContent =
            requirements.computing ?? "—";

        document.getElementById("e2e-activation-status").textContent =
            activation.status ?? "—";

        document.getElementById("e2e-deactivation-status").textContent =
            deactivation.status ?? "—";

        document.getElementById("e2e-initial-communication").textContent =
            initial.communication_available ?? "—";

        document.getElementById("e2e-initial-computing").textContent =
            initial.computing_available ?? "—";

        document.getElementById("e2e-activation-communication").textContent =
            activation.communication_available ?? "—";

        document.getElementById("e2e-activation-computing").textContent =
            activation.computing_available ?? "—";

        document.getElementById("e2e-release-communication").textContent =
            deactivation.communication_available ?? "—";

        document.getElementById("e2e-release-computing").textContent =
            deactivation.computing_available ?? "—";

        document.getElementById("e2e-activation-check").textContent =
            checks.activation_resource_check ?? "—";

        document.getElementById("e2e-release-check").textContent =
            checks.release_resource_check ?? "—";

        document.getElementById("e2e-experiment-result").textContent =
            data.experiment_result ?? "—";

        if (results) {
            results.style.display = "block";
        }

        if (status) {
            status.textContent = "COMPLETED — REAL NS-3 RESULTS LOADED";
        }

        sessionStorage.setItem(
            "iiitdmk_latest_experiment",
            "End-to-End Service Session"
        );

        sessionStorage.setItem(
            "iiitdmk_latest_status",
            "COMPLETED"
        );

    } catch (error) {

        console.error("End-to-end session error:", error);

        if (status) {
            status.textContent = "SIMULATION FAILED — " + error.message;
        }

    } finally {

        if (button) {
            button.disabled = false;
            button.textContent = "▶ RUN SESSION";
        }
    }
}

function showSimulatorSection(sectionId) {

    document.querySelectorAll(".simulator-section").forEach(section => {
        section.style.display = "none";
    });

    const target = document.getElementById(sectionId);

    if (target) {
        target.style.display = "block";
    }

    document.querySelectorAll(".nav-item").forEach(item => {
        item.classList.remove("active");
    });

    const activeNav = document.querySelector(
        `[onclick="showSimulatorSection('${sectionId}')"]`
    );

    if (activeNav) {
        activeNav.classList.add("active");
    }
    if (sectionId === "capabilities-section") {
        loadCapabilityRegistry();
        loadCapabilityResourceState();
    }

    if (sectionId === "services-section") {
        // Service panel is updated by the configured
        // Service Placement experiment.
    }

    if (sectionId === "results-section") {
        loadLatestResultsSummary();
    }
}



/* =========================================================
   SERVICES → REAL SERVICE PLACEMENT DATA
   ========================================================= */

async function loadServicePlacement() {

    const configuredCount =
        document.getElementById("service-configured-count");

    const sessionCount =
        document.getElementById("service-session-count");

    const serviceList =
        document.querySelector(".service-list");

    if (!serviceList) {
        return;
    }

    serviceList.innerHTML = `
        <div class="empty-state">
            <div class="empty-state-icon">◌</div>
            <strong>Loading service placement...</strong>
            <p>Running the service placement simulation.</p>
        </div>
    `;

    try {

        const response =
            await fetch("/api/services/placement");

        const data = await response.json();

        if (!data.success) {
            throw new Error(
                data.error || "Service placement simulation failed."
            );
        }

        if (configuredCount) {
            configuredCount.textContent = "1";
        }

        if (sessionCount) {
            sessionCount.textContent =
                data.service_active ? "1" : "0";
        }

        serviceList.innerHTML = `
            <div class="service-row">

                <div class="service-identity">
                    <span class="service-indicator"></span>

                    <div>
                        <strong>${data.service}</strong>
                        <small>
                            ${data.ue} → ${data.access_node} → ${data.selected_edge}
                        </small>
                    </div>
                </div>

                <span class="service-state">
                    ${data.service_active ? "ACTIVE" : "INACTIVE"}
                </span>

            </div>
        `;

        const placementCard =
            document.querySelector(
                "#services-section .discovery-entry"
            );

        if (placementCard) {

            const paragraph =
                placementCard.querySelector("p");

            if (paragraph) {
                paragraph.textContent =
                    `${data.service} placed on ${data.selected_edge}. ` +
                    `Compute remaining: ${data.compute_left}. ` +
                    `Communication remaining: ${data.communication_left}.`;
            }
        }

    } catch (error) {

        console.error(
            "Service placement error:",
            error
        );

        serviceList.innerHTML = `
            <div class="empty-state">
                <div class="empty-state-icon">!</div>
                <strong>Simulation failed</strong>
                <p>${error.message}</p>
            </div>
        `;

        if (configuredCount) {
            configuredCount.textContent = "—";
        }

        if (sessionCount) {
            sessionCount.textContent = "—";
        }
    }
}

/* =========================================================
   CAPABILITIES → REAL DYNAMIC RESOURCE STATE
   ========================================================= */

async function loadCapabilityResourceState() {

    const computeState =
        document.getElementById("capability-compute-state");

    if (!computeState) {
        return;
    }

    computeState.textContent = "LOADING...";

    try {

        const response =
            await fetch("/api/capabilities/resource-state");

        const data = await response.json();

        if (!data.success) {
            throw new Error(
                data.error || "Resource-state simulation failed."
            );
        }

        computeState.textContent =
            `${data.available_after_release} / ${data.capacity}`;

        const networkState =
            document.getElementById("capability-network-state");

        const aiState =
            document.getElementById("capability-ai-state");

        if (networkState) {
            networkState.textContent = "—";
        }

        if (aiState) {
            aiState.textContent = "—";
        }

        const computeCard =
            computeState.closest(".resource-state");

        if (computeCard) {

            const small =
                computeCard.querySelector("small");

            if (small) {
                small.textContent =
                    `${data.node} • ${data.resource} • ${Math.round(data.utilization_after_release * 100)}% utilized`;
            }
        }

    } catch (error) {

        console.error(
            "Dynamic resource-state error:",
            error
        );

        computeState.textContent = "ERROR";
    }
}

/* =========================================================
   RESULTS PAGE → EXISTING RESULT WORKSPACES
   ========================================================= */

function openDiscoveryResultsFromResultsPage() {

    showSimulatorSection("experiments-section");

    setTimeout(function() {
        runCapabilityDiscovery();
    }, 100);
}


function openScalabilityResultsFromResultsPage() {

    showSimulatorSection("experiments-section");

    setTimeout(function() {
        runCapabilityScalability();
    }, 100);
}

/* =========================================================
   CAPABILITIES PAGE → REAL NS-3 REGISTRY DATA
   ========================================================= */

async function loadCapabilityRegistry() {

    const list = document.getElementById("capability-list");
    const count = document.getElementById("capability-count");

    if (!list || !count) {
        return;
    }

    list.innerHTML = `
        <div class="empty-state">
            <div class="empty-state-icon">◌</div>
            <strong>Loading capabilities...</strong>
            <p>Running the capability registry simulation.</p>
        </div>
    `;

    try {

        const response = await fetch("/api/capabilities/registry");
        const data = await response.json();

        if (!data.success) {
            throw new Error(data.error || "Capability registry failed.");
        }

        count.textContent = data.total;

        if (!data.capabilities || data.capabilities.length === 0) {

            list.innerHTML = `
                <div class="empty-state">
                    <div class="empty-state-icon">◇</div>
                    <strong>No capabilities registered</strong>
                    <p>The simulator returned no registered capabilities.</p>
                </div>
            `;

            return;
        }

        list.innerHTML = data.capabilities.map(item => `
            <div class="capability-row">

                <div class="capability-identity">
                    <span class="capability-indicator"></span>

                    <div>
                        <strong>${item.capability}</strong>
                        <small>${item.node}</small>
                    </div>
                </div>

                <span class="capability-state">
                    ${item.state}
                </span>

            </div>
        `).join("");

        const computeState =
            document.getElementById("capability-compute-state");

        const networkState =
            document.getElementById("capability-network-state");

        const aiState =
            document.getElementById("capability-ai-state");

        if (computeState) {
            computeState.textContent =
                data.capabilities.some(
                    item => item.capability.toLowerCase() === "computing"
                )
                ? "AVAILABLE"
                : "—";
        }

        if (networkState) {
            networkState.textContent =
                data.capabilities.some(
                    item => item.capability.toLowerCase() === "communication"
                )
                ? "AVAILABLE"
                : "—";
        }

        if (aiState) {
            aiState.textContent =
                data.capabilities.some(
                    item => item.capability.toLowerCase() === "ai"
                )
                ? "AVAILABLE"
                : "—";
        }

    } catch (error) {

        console.error("Capability registry error:", error);

        list.innerHTML = `
            <div class="empty-state">
                <div class="empty-state-icon">!</div>
                <strong>Simulation failed</strong>
                <p>${error.message}</p>
            </div>
        `;

        count.textContent = "—";
    }
}

/* =========================================================
   CAPABILITIES → DYNAMIC CAPABILITY UPDATE
   ========================================================= */

async function runCapabilityUpdate() {

    const button =
        document.getElementById("capability-update-button");

    const results =
        document.getElementById("capability-update-results");

    if (!button || !results) {
        return;
    }

    button.disabled = true;
    button.textContent = "⏳ RUNNING...";

    results.innerHTML = `
        <div class="empty-state">
            <div class="empty-state-icon">↻</div>
            <strong>Running simulation...</strong>
            <p>Executing the real ns-3 capability update experiment.</p>
        </div>
    `;

    try {

        const response =
            await fetch("/api/capabilities/update");

        const data =
            await response.json();

        if (!data.success) {
            throw new Error(
                data.error || "Capability update simulation failed."
            );
        }

        results.innerHTML = `
            <div class="capability-update-summary">
                <span class="card-label">MINIMUM RESOURCE</span>
                <strong>${data.minimum_resource}</strong>
            </div>

            <div class="capability-update-timeline">
                ${data.states.map(state => {

                    const providers =
                        (state.providers || [])
                            .map(provider => `
                                <span class="update-provider">
                                    ${provider.provider}
                                    <b>${provider.available_resource}</b>
                                </span>
                            `)
                            .join("");

                    return `
                        <div class="capability-update-row">

                            <div class="update-time">
                                <span>t = ${state.time}s</span>
                            </div>

                            <div class="update-state">
                                <strong>
                                    ${state.state.replaceAll("_", " ")}
                                </strong>

                                <small>
                                    ${state.matches} matching provider(s)
                                </small>

                                <div class="update-providers">
                                    ${providers}
                                </div>
                            </div>

                        </div>
                    `;

                }).join("")}
            </div>
        `;

    } catch (error) {

        console.error(
            "Capability update error:",
            error
        );

        results.innerHTML = `
            <div class="empty-state">
                <div class="empty-state-icon">!</div>
                <strong>Simulation failed</strong>
                <p>${error.message}</p>
            </div>
        `;

    } finally {

        button.disabled = false;
        button.textContent = "▶ RUN UPDATE";
    }
}


/* =========================================================
   CAPABILITIES → CONSTRAINT-AWARE DISCOVERY
   ========================================================= */

async function runCapabilityConstraint() {

    const button =
        document.getElementById("capability-constraint-button");

    const results =
        document.getElementById("capability-constraint-results");

    if (!button || !results) {
        return;
    }

    button.disabled = true;
    button.textContent = "⏳ RUNNING...";

    results.innerHTML = `
        <div class="empty-state">
            <div class="empty-state-icon">↻</div>
            <strong>Running simulation...</strong>
            <p>Executing the real ns-3 constraint-aware discovery experiment.</p>
        </div>
    `;

    try {

        const response =
            await fetch("/api/capabilities/constraint");

        const data =
            await response.json();

        if (!data.success) {
            throw new Error(
                data.error || "Constraint simulation failed."
            );
        }

        const rows =
            (data.requests || []).map(request => `

                <div class="constraint-row">

                    <div class="constraint-request">
                        <span>${request.request}</span>
                        <strong>${request.constraint}</strong>
                    </div>

                    <div class="constraint-providers">
                        <span>
                            ${request.discovered} provider(s)
                        </span>

                        <div class="constraint-provider-list">
                            ${request.providers.map(provider => `
                                <span>${provider}</span>
                            `).join("")}
                        </div>
                    </div>

                    <span class="constraint-pass">
                        ${request.correctness}
                    </span>

                </div>

            `).join("");

        results.innerHTML = `

            <div class="constraint-summary">

                <div class="resource-state">
                    <span>REQUESTS</span>
                    <strong>
                        ${data.summary.discovery_requests ?? "—"}
                    </strong>
                </div>

                <div class="resource-state">
                    <span>PROVIDER MATCHES</span>
                    <strong>
                        ${data.summary.total_provider_matches ?? "—"}
                    </strong>
                </div>

                <div class="resource-state">
                    <span>CORRECTNESS</span>
                    <strong>
                        ${data.correctness_rate ?? "—"}%
                    </strong>
                </div>

            </div>

            <div class="constraint-list">
                ${rows}
            </div>
        `;

    } catch (error) {

        console.error(
            "Capability constraint error:",
            error
        );

        results.innerHTML = `
            <div class="empty-state">
                <div class="empty-state-icon">!</div>
                <strong>Simulation failed</strong>
                <p>${error.message}</p>
            </div>
        `;

    } finally {

        button.disabled = false;
        button.textContent = "▶ RUN CONSTRAINTS";
    }
}


/* =========================================================
   CAPABILITIES → TEMPORAL CAPABILITY FRESHNESS
   ========================================================= */
async function runCapabilityExperiment() {

    const button = document.getElementById("capability-experiment-button");
    const status = document.getElementById("capability-experiment-status");
    const results = document.getElementById("capability-experiment-results");

    if (button) {
        button.disabled = true;
        button.textContent = "⏳ RUNNING...";
    }

    if (status) {
        status.textContent = "RUNNING REAL NS-3 CAPABILITY EXPERIMENT...";
    }

    if (results) {
        results.style.display = "none";
    }

    try {

        const response = await fetch("/api/capabilities/experiment");
        const data = await response.json();

        if (!response.ok || !data.success) {
            throw new Error(data.error || "Capability experiment failed.");
        }

        const experiment = data.experiment || {};
        const summary = data.summary || {};
        const comparison = data.comparison || {};
        const requests = data.requests || {};

        document.getElementById("capability-experiment-profile").textContent =
            experiment.workload_profile ?? "—";

        document.getElementById("capability-experiment-updates").textContent =
            experiment.runtime_updates ?? "—";

        document.getElementById("capability-experiment-count").textContent =
            experiment.request_count ?? "—";

        const staticSummary = summary.STATIC || {};
        const dynamicSummary = summary.DYNAMIC || {};

        document.getElementById("capability-static-success").textContent =
            staticSummary.success_rate ?? "—";

        document.getElementById("capability-static-failure").textContent =
            staticSummary.failure_rate ?? "—";

        document.getElementById("capability-static-allocated").textContent =
            staticSummary.allocated_compute ?? "—";

        document.getElementById("capability-static-allocation").textContent =
            staticSummary.allocation_ratio ?? "—";

        document.getElementById("capability-dynamic-success").textContent =
            dynamicSummary.success_rate ?? "—";

        document.getElementById("capability-dynamic-failure").textContent =
            dynamicSummary.failure_rate ?? "—";

        document.getElementById("capability-dynamic-allocated").textContent =
            dynamicSummary.allocated_compute ?? "—";

        document.getElementById("capability-dynamic-allocation").textContent =
            dynamicSummary.allocation_ratio ?? "—";

        document.getElementById("capability-success-difference").textContent =
            comparison.success_rate_difference ?? "—";

        document.getElementById("capability-allocation-difference").textContent =
            comparison.allocation_ratio_difference ?? "—";

        renderCapabilityExperimentRequests(
            "capability-static-requests",
            requests.STATIC || [],
            false
        );

        renderCapabilityExperimentRequests(
            "capability-dynamic-requests",
            requests.DYNAMIC || [],
            true
        );

        if (results) {
            results.style.display = "block";
        }

        if (status) {
            status.textContent = "COMPLETED — REAL NS-3 RESULTS LOADED";
        }

        sessionStorage.setItem(
            "iiitdmk_latest_experiment",
            "Dynamic Capability Discovery vs Static Selection"
        );

        sessionStorage.setItem(
            "iiitdmk_latest_status",
            "COMPLETED"
        );

    } catch (error) {

        console.error("Capability experiment error:", error);

        if (status) {
            status.textContent = "SIMULATION FAILED — " + error.message;
        }

    } finally {

        if (button) {
            button.disabled = false;
            button.textContent = "▶ RUN EXPERIMENT";
        }
    }
}


function renderCapabilityExperimentRequests(containerId, requests, dynamicMode) {

    const container = document.getElementById(containerId);

    if (!container) {
        return;
    }

    if (!requests.length) {
        container.innerHTML = '<div class="empty-state">No request data returned.</div>';
        return;
    }

    let html = `
        <div class="experiment-request-table">
            <div class="request-row request-header-row">
                <span>REQ</span>
                <span>REQUIRED</span>
                <span>SELECTED</span>
                ${dynamicMode ? "<span>CANDIDATES</span>" : ""}
                <span>RESULT</span>
            </div>
    `;

    requests.forEach((request, index) => {

        const result = request.result || "—";
        const resultClass =
            result === "SUCCESS" ? "request-success" : "request-failure";

        html += `
            <div class="request-row">
                <span>${index + 1}</span>
                <span>${request.required ?? "—"}</span>
                <span>${request.selected ?? "—"}</span>
                ${dynamicMode ? `<span>${request.candidates ?? "—"}</span>` : ""}
                <span class="${resultClass}">${result}</span>
            </div>
        `;
    });

    html += "</div>";

    container.innerHTML = html;
}


async function runCapabilityFreshness() {
    const button = document.getElementById("capability-freshness-button");
    const results = document.getElementById("capability-freshness-results");

    const nodeAValidity =
        parseFloat(document.getElementById("freshness-node-a-validity").value);

    const nodeBValidity =
        parseFloat(document.getElementById("freshness-node-b-validity").value);

    const minimumResource =
        parseFloat(document.getElementById("freshness-minimum-resource").value);

    const secondCheckpoint =
        parseFloat(document.getElementById("freshness-second-checkpoint").value);

    const thirdCheckpoint =
        parseFloat(document.getElementById("freshness-third-checkpoint").value);

    if (
        !Number.isFinite(nodeAValidity) ||
        !Number.isFinite(nodeBValidity) ||
        !Number.isFinite(minimumResource) ||
        !Number.isFinite(secondCheckpoint) ||
        !Number.isFinite(thirdCheckpoint)
    ) {
        results.innerHTML = `
            <div class="empty-state">
                <div class="empty-state-icon">!</div>
                <strong>Invalid parameters</strong>
                <p>Please enter valid numeric values.</p>
            </div>
        `;
        return;
    }

    if (secondCheckpoint <= 0 || thirdCheckpoint <= secondCheckpoint) {
        results.innerHTML = `
            <div class="empty-state">
                <div class="empty-state-icon">!</div>
                <strong>Invalid checkpoints</strong>
                <p>Third checkpoint must be greater than the second checkpoint.</p>
            </div>
        `;
        return;
    }

    button.disabled = true;
    button.textContent = "⏳ RUNNING...";

    results.innerHTML = `
        <div class="empty-state">
            <div class="empty-state-icon">◷</div>
            <strong>Running experiment...</strong>
            <p>Executing the real ns-3 capability freshness experiment.</p>
        </div>
    `;

    try {
        const params = new URLSearchParams({
            nodeAValidity: nodeAValidity,
            nodeBValidity: nodeBValidity,
            minimumResource: minimumResource,
            secondCheckpoint: secondCheckpoint,
            thirdCheckpoint: thirdCheckpoint
        });

        const response = await fetch(
            `/api/capabilities/freshness?${params.toString()}`
        );

        const data = await response.json();

        if (!response.ok || !data.success) {
            throw new Error(data.error || "Freshness experiment failed.");
        }

        const checkpoints = data.checkpoints || [];

        let checkpointHTML = "";

        checkpoints.forEach((checkpoint) => {
            checkpointHTML += `
                <div class="freshness-checkpoint">
                    <div class="freshness-checkpoint-header">
                        <strong>${checkpoint.label}</strong>
                        <span>
                            Fresh: ${checkpoint.fresh_matches}
                            / ${checkpoint.normal_matches}
                        </span>
                    </div>

                    <div class="freshness-metrics">
                        <div>
                            <span class="metric-label">Normal discovery</span>
                            <strong>${checkpoint.normal_matches}</strong>
                        </div>

                        <div>
                            <span class="metric-label">Fresh-only discovery</span>
                            <strong>${checkpoint.fresh_matches}</strong>
                        </div>
                    </div>

                    <div class="freshness-providers">
                        <span class="metric-label">Fresh providers</span>
                        <p>
                            ${
                                checkpoint.fresh_providers.length
                                    ? checkpoint.fresh_providers.join(", ")
                                    : "None"
                            }
                        </p>
                    </div>
                </div>
            `;
        });

        results.innerHTML = `
            <div class="experiment-result-header">
                <div>
                    <span class="card-label">EXPERIMENT COMPLETED</span>
                    <h4>Temporal Capability Freshness</h4>
                </div>

                <span class="status-badge success">VERIFIED</span>
            </div>

            <div class="freshness-summary">
                <div>
                    <span>Node A validity</span>
                    <strong>${data.parameters.nodeAValidity}s</strong>
                </div>

                <div>
                    <span>Node B validity</span>
                    <strong>${data.parameters.nodeBValidity}s</strong>
                </div>

                <div>
                    <span>Minimum resource</span>
                    <strong>${data.parameters.minimumResource}</strong>
                </div>

                <div>
                    <span>Checkpoints</span>
                    <strong>${checkpoints.length}</strong>
                </div>
            </div>

            <div class="freshness-checkpoints">
                ${checkpointHTML}
            </div>

            <div class="experiment-success-message">
                ✓ Freshness behavior verified using real ns-3 simulation output.
            </div>
        `;

        sessionStorage.setItem(
            "iiitdmk_latest_experiment",
            "Capability Freshness"
        );

        sessionStorage.setItem(
            "iiitdmk_latest_status",
            "COMPLETED"
        );

    } catch (error) {
        results.innerHTML = `
            <div class="empty-state">
                <div class="empty-state-icon">!</div>
                <strong>Simulation failed</strong>
                <p>${error.message}</p>
            </div>
        `;
    } finally {
        button.disabled = false;
        button.textContent = "▶ RUN FRESHNESS";
    }
}


async function runMobilityHandover() {
    const status = document.getElementById("mobility-status");
    const button = document.getElementById("mobility-run-button");

    const ueX = Number(document.getElementById("mobility-ue-x").value);
    const ueY = Number(document.getElementById("mobility-ue-y").value);
    const access1X = Number(document.getElementById("mobility-access1-x").value);
    const access1Y = Number(document.getElementById("mobility-access1-y").value);
    const access2X = Number(document.getElementById("mobility-access2-x").value);
    const access2Y = Number(document.getElementById("mobility-access2-y").value);

    if (![ueX, ueY, access1X, access1Y, access2X, access2Y]
        .every(Number.isFinite)) {
        status.textContent = "INVALID PARAMETERS";
        return;
    }

    status.textContent = "RUNNING...";
    button.disabled = true;

    try {
        const params = new URLSearchParams({
            ueX,
            ueY,
            access1X,
            access1Y,
            access2X,
            access2Y
        });

        const response = await fetch(
            `/api/network/mobility-handover?${params.toString()}`
        );

        const data = await response.json();

        if (!response.ok || !data.success) {
            throw new Error(
                data.error || "Mobility handover simulation failed."
            );
        }

        document.getElementById("mobility-ue-position").textContent =
            `(${data.parameters.ueX}, ${data.parameters.ueY})`;

        document.getElementById("mobility-distance1").textContent =
            Number(data.distance_access1).toFixed(2);

        document.getElementById("mobility-distance2").textContent =
            Number(data.distance_access2).toFixed(2);

        document.getElementById("mobility-handover-status").textContent =
            data.handover_required ? "YES" : "NO";

        document.getElementById("mobility-serving-node").textContent =
            data.serving_access_node || "—";

        document.getElementById("mobility-edge-node").textContent =
            data.reachable_edge_node || "—";

        document.getElementById("mobility-result").textContent =
            data.handover_status || "—";

        status.textContent = "COMPLETED";

        sessionStorage.setItem(
            "iiitdmk_latest_experiment",
            "Mobility + Handover"
        );
        sessionStorage.setItem(
            "iiitdmk_latest_status",
            "COMPLETED"
        );

    } catch (error) {
        console.error(error);
        status.textContent = "FAILED";
        alert(error.message);
    } finally {
        button.disabled = false;
    }
}



async function runMobilityHandoverExperiment() {
    const status = document.getElementById("mobility-experiment-status");
    const button = document.getElementById("mobility-handover-experiment-button");

    const access1Capacity =
        Number(document.getElementById("mobility-exp-access1-capacity").value);

    const access2Capacity =
        Number(document.getElementById("mobility-exp-access2-capacity").value);

    const serviceCommunication =
        Number(document.getElementById("mobility-exp-service-communication").value);

    const serviceCompute =
        Number(document.getElementById("mobility-exp-service-compute").value);

    if (
        ![access1Capacity, access2Capacity, serviceCommunication, serviceCompute]
            .every(Number.isFinite)
    ) {
        status.textContent = "INVALID PARAMETERS";
        return;
    }

    status.textContent = "RUNNING...";
    button.disabled = true;

    try {
        const params = new URLSearchParams({
            access1CommunicationCapacity: access1Capacity,
            access2CommunicationCapacity: access2Capacity,
            serviceCommunicationRequirement: serviceCommunication,
            serviceComputeRequirement: serviceCompute
        });

        const response = await fetch(
            `/api/network/mobility-handover-experiment?${params.toString()}`
        );

        const data = await response.json();

        document.getElementById("mobility-experiment-results").style.display = "block";

        document.getElementById("mobility-exp-initial").textContent =
            data.initial_activation || "—";

        document.getElementById("mobility-exp-old-release").textContent =
            data.old_session_release || "—";

        document.getElementById("mobility-exp-new-access").textContent =
            data.new_access_connection || "—";

        document.getElementById("mobility-exp-new-session").textContent =
            data.new_session_activation || "—";

        document.getElementById("mobility-exp-old-resource").textContent =
            data.old_resource_restored || "—";

        document.getElementById("mobility-exp-new-resource").textContent =
            data.new_resource_reserved || "—";

        document.getElementById("mobility-exp-result").textContent =
            data.experiment_status || "—";

        status.textContent = data.success
            ? "COMPLETED"
            : "COMPLETED — FAILED";

        sessionStorage.setItem(
            "iiitdmk_latest_experiment",
            "Mobility + Handover Experiment"
        );

        sessionStorage.setItem(
            "iiitdmk_latest_status",
            data.success ? "COMPLETED" : "FAILED"
        );

    } catch (error) {
        console.error(error);
        status.textContent = "FAILED";
        alert(error.message);
    } finally {
        button.disabled = false;
    }
}

/* =========================================================
   IIITDMK 6G SIMULATOR — EXPERIMENT PAGE CONTROLLERS
   ========================================================= */

async function runExperimentAPI(url, params, resultId, buttonId) {
    const result = document.getElementById(resultId);
    const button = document.getElementById(buttonId);

    if (!result || !button) return;

    const originalText = button.innerHTML;
    button.disabled = true;
    button.innerHTML = "Running simulation...";

    if (result) {
        result.innerHTML = '<span>Running ns-3 experiment...</span>';
    }

    try {
        const query = new URLSearchParams(params);
        const response = await fetch(`${url}?${query.toString()}`);
        const data = await response.json();

        if (!response.ok || data.error) {
            throw new Error(data.error || "Simulation failed");
        }

        result.innerHTML = formatExperimentResult(data);
    } catch (error) {
        result.innerHTML =
            `<span style="color:#ef6b73;">Simulation failed: ${escapeExperimentText(error.message)}</span>`;
    } finally {
        button.disabled = false;
        button.innerHTML = originalText;
    }
}

function escapeExperimentText(value) {
    return String(value)
        .replace(/&/g, "&amp;")
        .replace(/</g, "&lt;")
        .replace(/>/g, "&gt;")
        .replace(/"/g, "&quot;")
        .replace(/'/g, "&#039;");
}

function formatExperimentResult(data) {

    if (data.output) {
        const output = String(data.output);

        const staticSuccess =
            output.match(/STATIC SELECTION[\s\S]*?Success rate\s*:\s*([\d.]+)%/);

        const dynamicSuccess =
            output.match(/DYNAMIC CAPABILITY DISCOVERY[\s\S]*?Success rate\s*:\s*([\d.]+)%/);

        const staticAllocation =
            output.match(/STATIC SELECTION[\s\S]*?Allocation ratio\s*:\s*([\d.]+)%/);

        const dynamicAllocation =
            output.match(/DYNAMIC CAPABILITY DISCOVERY[\s\S]*?Allocation ratio\s*:\s*([\d.]+)%/);

        const comparison =
            output.match(/Success-rate difference\s*:\s*([\d.]+)\s*percentage points/);

        const allocationDifference =
            output.match(/Allocation-ratio diff\s*:\s*([\d.]+)\s*percentage points/);

        if (staticSuccess && dynamicSuccess) {
            return `
                <div class="experiment-result-box">

                    <div class="result-title">
                        ✓ Experiment Completed
                    </div>

                    <div class="result-subtitle">
                        Dynamic Capability Discovery vs Static Selection
                    </div>

                    <div class="result-comparison">

                        <div class="result-column">
                            <span class="result-label">STATIC SELECTION</span>
                            <strong>${staticSuccess[1]}%</strong>
                            <small>Success Rate</small>
                            ${
                                staticAllocation
                                ? `<span>${staticAllocation[1]}% allocation</span>`
                                : ""
                            }
                        </div>

                        <div class="result-vs">VS</div>

                        <div class="result-column dynamic-result">
                            <span class="result-label">DYNAMIC DISCOVERY</span>
                            <strong>${dynamicSuccess[1]}%</strong>
                            <small>Success Rate</small>
                            ${
                                dynamicAllocation
                                ? `<span>${dynamicAllocation[1]}% allocation</span>`
                                : ""
                            }
                        </div>

                    </div>

                    ${
                        comparison
                        ? `
                        <div class="result-highlight">
                            <span>Success-rate improvement</span>
                            <strong>+${comparison[1]} percentage points</strong>
                        </div>
                        `
                        : ""
                    }

                    ${
                        allocationDifference
                        ? `
                        <div class="result-highlight secondary">
                            <span>Allocation-ratio improvement</span>
                            <strong>+${allocationDifference[1]} percentage points</strong>
                        </div>
                        `
                        : ""
                    }

                    <details class="raw-result">
                        <summary>View detailed simulation output</summary>
                        <pre>${escapeExperimentText(output)}</pre>
                    </details>

                </div>
            `;
        }

        return `
            <div class="experiment-result-box">
                <div class="result-title">✓ Experiment Completed</div>
                <details class="raw-result">
                    <summary>View simulation output</summary>
                    <pre>${escapeExperimentText(output)}</pre>
                </details>
            </div>
        `;
    }

    return `
        <div class="experiment-result-box">
            <div class="result-title">✓ Experiment Completed</div>
        </div>
    `;
}


/* Dynamic Capability Discovery */
document.getElementById("experiments-discovery-button")
?.addEventListener("click", () => {

    const requests =
        document.getElementById("discovery-requests")?.value || 10;

    const profile =
        document.getElementById("discovery-profile")?.value || 1;

    const updates =
        document.getElementById("discovery-updates")?.value || 1;

    runExperimentAPI(
        "/api/experiment/capability-discovery",
        {
            requests,
            profile,
            updates
        },
        "experiments-discovery-result",
        "experiments-discovery-button"
    );
});


/* Capability Constraint */
document.getElementById("experiments-constraint-button")
?.addEventListener("click", async () => {

    const button = document.getElementById("experiments-constraint-button");
    const result = document.getElementById("experiments-constraint-result");

    button.disabled = true;
    button.innerHTML = "Running simulation...";
    result.innerHTML = "Running constraint-aware capability discovery...";

    try {
        const minimumResource =
        document.getElementById("constraint-minimum-resource")?.value || "80";

    const response = await fetch(
        `/api/capabilities/constraint?minimumResource=${encodeURIComponent(minimumResource)}`
    );
        const data = await response.json();

        if (!response.ok || data.error) {
            throw new Error(data.error || "Constraint experiment failed");
        }

        const output = String(data.raw_output || data.output || "");

        const pattern =
            /Request\s+(\d+)\s+\|\s+([^\n]+)\n\s*Discovered providers\s*:\s*(\d+)\n\s*Providers\s*:\s*([^\n]+)\n\s*Expected providers\s*:\s*([^\n]+)\n\s*Discovery correctness\s*:\s*(PASS|FAIL)/g;

        const rows = [];
        let match;

        while ((match = pattern.exec(output)) !== null) {
            rows.push({
                request: match[1],
                constraint: match[2].trim(),
                providers: match[3],
                correctness: match[6]
            });
        }

        const totalRequests =
            data.discovery_requests ?? rows.length;

        const correct =
            data.correct_discoveries ??
            rows.filter(row => row.correctness === "PASS").length;

        const totalMatches =
            data.total_provider_matches ??
            rows.reduce((sum, row) => sum + Number(row.providers), 0);

        const rate =
            data.correctness_rate ??
            (totalRequests ? (correct / totalRequests) * 100 : 0);

        result.innerHTML = `
            <div class="experiment-result-box">

                <div class="result-title">
                    ✓ Constraint Discovery Completed
                </div>

                <div class="result-subtitle">
                    Capability discovery under multiple constraints
                </div>

                ${
                    rows.length
                    ? `
                    <table class="scalability-result-table">
                        <thead>
                            <tr>
                                <th>Request</th>
                                <th>Constraint</th>
                                <th>Providers</th>
                                <th>Correctness</th>
                            </tr>
                        </thead>
                        <tbody>
                            ${rows.map(row => `
                                <tr>
                                    <td>${row.request}</td>
                                    <td style="text-align:left;">
                                        ${escapeExperimentText(row.constraint)}
                                    </td>
                                    <td>${row.providers}</td>
                                    <td>
                                        <strong>
                                            ${row.correctness === "PASS"
                                                ? "✓ PASS"
                                                : "✗ FAIL"}
                                        </strong>
                                    </td>
                                </tr>
                            `).join("")}
                        </tbody>
                    </table>
                    `
                    : `
                    <div class="result-subtitle">
                        ${correct} of ${totalRequests} discovery requests were correct.
                    </div>
                    `
                }

                <div class="scalability-summary">
                    <span>${correct} / ${totalRequests} correct</span>
                    <span>${totalMatches} provider matches</span>
                    <span>${Number(rate).toFixed(2)}% correctness</span>
                </div>

                <details class="raw-result">
                    <summary>View detailed simulation output</summary>
                    <pre>${escapeExperimentText(output)}</pre>
                </details>

            </div>
        `;

    } catch (error) {
        result.innerHTML =
            `<span style="color:#ef6b73;">
                Simulation failed: ${escapeExperimentText(error.message)}
            </span>`;
    } finally {
        button.disabled = false;
        button.innerHTML = "Run Experiment →";
    }
});


/* Capability Freshness */
document.getElementById("experiments-freshness-button")
?.addEventListener("click", async () => {

    const button = document.getElementById("experiments-freshness-button");
    const result = document.getElementById("experiments-freshness-result");

    const params = {
        nodeAValidity:
            document.getElementById("freshness-node-a")?.value || 4,

        nodeBValidity:
            document.getElementById("freshness-node-b")?.value || 10,

        minimumResource:
            document.getElementById("freshness-min-resource")?.value || 50,

        secondCheckpoint:
            document.getElementById("freshness-checkpoint-2")?.value || 6,

        thirdCheckpoint:
            document.getElementById("freshness-checkpoint-3")?.value || 11
    };

    button.disabled = true;
    button.innerHTML = "Running simulation...";
    result.innerHTML = "Running capability freshness experiment...";

    try {
        const query = new URLSearchParams(params).toString();

        const response = await fetch(
            `/api/capabilities/freshness?${query}`
        );

        const data = await response.json();

        if (!response.ok || data.error) {
            throw new Error(data.error || "Freshness experiment failed");
        }

        const output = String(
            data.raw_output || data.output || ""
        );

        result.innerHTML = `
            <div class="experiment-result-box">

                <div class="result-title">
                    ✓ Capability Freshness Experiment Completed
                </div>

                <div class="result-subtitle">
                    Capability validity under configurable time checkpoints
                </div>

                <div class="scalability-summary">

                    <span>
                        Node A validity: ${escapeExperimentText(String(params.nodeAValidity))}
                    </span>

                    <span>
                        Node B validity: ${escapeExperimentText(String(params.nodeBValidity))}
                    </span>

                    <span>
                        Minimum resource: ${escapeExperimentText(String(params.minimumResource))}
                    </span>

                    <span>
                        Checkpoint 2: ${escapeExperimentText(String(params.secondCheckpoint))}
                    </span>

                    <span>
                        Checkpoint 3: ${escapeExperimentText(String(params.thirdCheckpoint))}
                    </span>

                </div>

                <details class="raw-result" open>
                    <summary>View simulation result</summary>
                    <pre>${escapeExperimentText(output)}</pre>
                </details>

            </div>
        `;

    } catch (error) {
        result.innerHTML =
            `<span style="color:#ef6b73;">
                Simulation failed: ${escapeExperimentText(error.message)}
            </span>`;
    } finally {
        button.disabled = false;
        button.innerHTML = "Run Experiment →";
    }
});


/* Capability Update */
document.getElementById("experiments-update-button")
?.addEventListener("click", () => {

    runExperimentAPI(
        "/api/capabilities/update",
        {},
        "experiments-update-result",
        "experiments-update-button"
    );
});


/* Capability Scalability */
document.getElementById("experiments-scalability-button")
?.addEventListener("click", async () => {

    const button = document.getElementById("experiments-scalability-button");
    const result = document.getElementById("experiments-scalability-result");

    const repetitions =
        document.getElementById("scalability-repetitions")?.value || 1000;

    button.disabled = true;
    button.innerHTML = "Running simulation...";
    result.innerHTML = "Running ns-3 scalability experiment...";

    try {
        const response = await fetch(
            `/api/experiment/capability-scalability?repetitions=${encodeURIComponent(repetitions)}`
        );

        const data = await response.json();

        if (!response.ok || data.error) {
            throw new Error(data.error || "Scalability experiment failed");
        }

        const output = String(data.output || "");

        const rows = [];
        const pattern =
            /(\d+),(\d+),([\d.]+),([\d.]+),(\d+)/g;

        let match;

        while ((match = pattern.exec(output)) !== null) {
            rows.push({
                providers: match[1],
                repetitions: match[2],
                total: match[3],
                average: match[4],
                matches: match[5]
            });
        }

        if (!rows.length) {
            result.innerHTML = `
                <div class="experiment-result-box">
                    <div class="result-title">✓ Experiment Completed</div>
                    <details class="raw-result">
                        <summary>View simulation output</summary>
                        <pre>${escapeExperimentText(output)}</pre>
                    </details>
                </div>
            `;
        } else {
            result.innerHTML = `
                <div class="experiment-result-box">

                    <div class="result-title">
                        ✓ Scalability Experiment Completed
                    </div>

                    <div class="result-subtitle">
                        Capability discovery performance as provider population increases
                    </div>

                    <table class="scalability-result-table">
                        <thead>
                            <tr>
                                <th>Providers</th>
                                <th>Repetitions</th>
                                <th>Avg. Discovery</th>
                                <th>Total Matches</th>
                            </tr>
                        </thead>
                        <tbody>
                            ${rows.map(row => `
                                <tr>
                                    <td>${row.providers}</td>
                                    <td>${row.repetitions}</td>
                                    <td>${row.average} μs</td>
                                    <td>${row.matches}</td>
                                </tr>
                            `).join("")}
                        </tbody>
                    </table>

                    <div class="scalability-summary">
                        <span>${rows.length} provider scales tested</span>
                        <span>${repetitions} repetitions / scale</span>
                    </div>

                    <details class="raw-result">
                        <summary>View detailed simulation output</summary>
                        <pre>${escapeExperimentText(output)}</pre>
                    </details>

                </div>
            `;
        }

    } catch (error) {
        result.innerHTML =
            `<span style="color:#ef6b73;">Simulation failed: ${escapeExperimentText(error.message)}</span>`;
    } finally {
        button.disabled = false;
        button.innerHTML = "Run Experiment →";
    }
});



/* Mobility */
document.getElementById("experiments-mobility-button")
?.addEventListener("click", () => {

    runExperimentAPI(
        "/api/network/mobility-handover",
        {
            ueX: document.getElementById("mobility-ue-x")?.value || 25,
            ueY: document.getElementById("mobility-ue-y")?.value || 0,
            access1X: document.getElementById("mobility-a1-x")?.value || 0,
            access1Y: 0,
            access2X: document.getElementById("mobility-a2-x")?.value || 100,
            access2Y: 0
        },
        "experiments-mobility-result",
        "experiments-mobility-button"
    );
});


/* Mobility + Handover Experiment */
document.getElementById("experiments-handover-button")
?.addEventListener("click", async () => {

    const button = document.getElementById("experiments-handover-button");
    const result = document.getElementById("experiments-handover-result");

    const params = {
        access1CommunicationCapacity:
            document.getElementById("handover-a1-capacity")?.value || 100,

        access2CommunicationCapacity:
            document.getElementById("handover-a2-capacity")?.value || 100,

        serviceCommunicationRequirement:
            document.getElementById("handover-communication")?.value || 20,

        serviceComputeRequirement:
            document.getElementById("handover-computing")?.value || 20
    };

    button.disabled = true;
    button.innerHTML = "Running simulation...";
    result.innerHTML = "Running mobility + handover experiment...";

    try {
        const query = new URLSearchParams(params).toString();

        const response = await fetch(
            `/api/network/mobility-handover-experiment?${query}`
        );

        const data = await response.json();

        if (!response.ok || data.error) {
            throw new Error(data.error || "Handover experiment failed");
        }

        const rows = [
            ["Initial Activation", data.initial_activation],
            ["Old Session Release", data.old_session_release],
            ["New Access Connection", data.new_access_connection],
            ["New Session Activation", data.new_session_activation],
            ["Old Resource Restored", data.old_resource_restored],
            ["New Resource Reserved", data.new_resource_reserved],
            ["Experiment Result", data.experiment_status]
        ];

        result.innerHTML = `
            <div class="experiment-result-box">
                <div class="result-title">✓ Mobility + Handover Completed</div>

                <div class="handover-result-grid">
                    ${rows.map(([label, value]) => `
                        <div class="handover-result-row">
                            <span>${label}</span>
                            <strong>${escapeExperimentText(
                                value === undefined || value === null
                                    ? "—"
                                    : String(value)
                            )}</strong>
                        </div>
                    `).join("")}
                </div>

                <details class="raw-result">
                    <summary>View detailed simulation output</summary>
                    <pre>${escapeExperimentText(
                        data.raw_output || data.output || ""
                    )}</pre>
                </details>
            </div>
        `;

    } catch (error) {
        result.innerHTML =
            `<span style="color:#ef6b73;">Simulation failed: ${escapeExperimentText(error.message)}</span>`;
    } finally {
        button.disabled = false;
        button.innerHTML = "RUN EXPERIMENT";
    }
});


document.addEventListener("DOMContentLoaded", () => {
    const resourceStressButton = document.getElementById(
        "experiments-resource-stress-button"
    );

    if (resourceStressButton) {
        resourceStressButton.addEventListener(
            "click",
            runResourceStressExperiment
        );
    }
});


// ---------------------------------------------------------------------------
// Heterogeneous Network
// ---------------------------------------------------------------------------

async function runHeterogeneousNetworkExperiment() {

    const panel =
        document.getElementById("heterogeneous-network-result-panel");

    const output =
        document.getElementById("heterogeneous-network-result");

    const outputStatus =
        document.getElementById("heterogeneous-network-result-status");

    const access1Communication =
        document.getElementById(
            "heterogeneous-access1-communication"
        ).value;

    const access2Communication =
        document.getElementById(
            "heterogeneous-access2-communication"
        ).value;

    const edge1Computing =
        document.getElementById(
            "heterogeneous-edge1-computing"
        ).value;

    const edge2Computing =
        document.getElementById(
            "heterogeneous-edge2-computing"
        ).value;

    const edge3Computing =
        document.getElementById(
            "heterogeneous-edge3-computing"
        ).value;

    panel.style.display = "block";

    outputStatus.textContent = "RUNNING";
    outputStatus.className = "experiment-status";

    output.textContent =
        "Running heterogeneous network experiment...";

    panel.scrollIntoView({
        behavior: "smooth",
        block: "nearest"
    });

    const params = new URLSearchParams({
        access1CommunicationCapacity: access1Communication,
        access2CommunicationCapacity: access2Communication,
        edge1ComputingCapacity: edge1Computing,
        edge2ComputingCapacity: edge2Computing,
        edge3ComputingCapacity: edge3Computing
    });

    try {

        const response = await fetch(
            `/api/experiment/heterogeneous-network?${params.toString()}`
        );

        const data = await response.json();

        if (data.success) {

            outputStatus.textContent = "PASSED";
            outputStatus.className =
                "experiment-status success";

            output.textContent =
                data.output ||
                "Experiment completed successfully.";

        } else {

            outputStatus.textContent = "FAILED";
            outputStatus.className =
                "experiment-status failed";

            output.textContent =
                data.output ||
                data.error ||
                "Experiment validation failed.";
        }

    } catch (error) {

        outputStatus.textContent = "ERROR";
        outputStatus.className =
            "experiment-status failed";

        output.textContent =
            "Unable to execute experiment.\n\n" +
            error.message;
    }
}


async function runDynamicResourceStateExperiment() {
    const panel =
        document.getElementById("dynamic-resource-state-result-panel");

    const output =
        document.getElementById("dynamic-resource-state-result");

    const outputStatus =
        document.getElementById("dynamic-resource-state-result-status");

    const capacity =
        document.getElementById("dynamic-resource-capacity").value;

    const consumption =
        document.getElementById("dynamic-resource-consumption").value;

    const release =
        document.getElementById("dynamic-resource-release").value;

    panel.style.display = "block";

    outputStatus.textContent = "RUNNING";
    outputStatus.className = "experiment-status";

    output.textContent =
        "Running dynamic resource state experiment...";

    panel.scrollIntoView({
        behavior: "smooth",
        block: "nearest"
    });

    const params = new URLSearchParams({
        capacity: capacity,
        consumption: consumption,
        release: release
    });

    try {
        const response = await fetch(
            `/api/experiment/dynamic-resource-state?${params.toString()}`
        );

        const data = await response.json();

        if (data.success) {
            outputStatus.textContent = "PASSED";
            outputStatus.className =
                "experiment-status success";

            output.textContent =
                data.output ||
                "Experiment completed successfully.";
        } else {
            outputStatus.textContent = "FAILED";
            outputStatus.className =
                "experiment-status failed";

            output.textContent =
                data.output ||
                data.error ||
                "Experiment validation failed.";
        }

    } catch (error) {
        outputStatus.textContent = "ERROR";
        outputStatus.className =
            "experiment-status failed";

        output.textContent =
            "Unable to execute experiment.\n\n" +
            error.message;
    }
}


/* =========================================================
   EXPERIMENT CARD → CAPABILITY REGISTRY
   ========================================================= */

async function runCapabilityRegistryExperiment() {

    const button =
        document.getElementById("capability-registry-button");

    const result =
        document.getElementById("capability-registry-result");

    const communication =
        document.getElementById(
            "registry-communication-providers"
        );

    const sensing =
        document.getElementById(
            "registry-sensing-providers"
        );

    const computing =
        document.getElementById(
            "registry-computing-providers"
        );

    const ai =
        document.getElementById(
            "registry-ai-providers"
        );

    const query =
        document.getElementById(
            "registry-query-type"
        );

    if (!button || !result ||
        !communication || !sensing ||
        !computing || !ai || !query) {
        return;
    }

    const communicationProviders =
        Number(communication.value);

    const sensingProviders =
        Number(sensing.value);

    const computingProviders =
        Number(computing.value);

    const aiProviders =
        Number(ai.value);

    const queryType =
        query.value;

    const totalProviders =
        communicationProviders +
        sensingProviders +
        computingProviders +
        aiProviders;

    if (
        !Number.isInteger(communicationProviders) ||
        !Number.isInteger(sensingProviders) ||
        !Number.isInteger(computingProviders) ||
        !Number.isInteger(aiProviders) ||
        communicationProviders < 0 ||
        sensingProviders < 0 ||
        computingProviders < 0 ||
        aiProviders < 0
    ) {
        result.innerHTML = `
            <div class="empty-state">
                <strong>Invalid configuration</strong>
                <p>
                    Provider counts must be non-negative integers.
                </p>
            </div>
        `;
        return;
    }

    if (totalProviders === 0) {
        result.innerHTML = `
            <div class="empty-state">
                <strong>Invalid configuration</strong>
                <p>
                    At least one capability provider is required.
                </p>
            </div>
        `;
        return;
    }

    button.disabled = true;
    button.textContent = "⏳ RUNNING...";

    result.className = "experiment-inline-result";
    result.innerHTML = `
        <div class="empty-state">
            <strong>Running capability registry...</strong>
            <p>
                Applying the configured registry to ns-3.
            </p>
        </div>
    `;

    try {

        const params = new URLSearchParams({
            communicationProviders:
                communicationProviders,
            sensingProviders:
                sensingProviders,
            computingProviders:
                computingProviders,
            aiProviders:
                aiProviders,
            queryType:
                queryType
        });

        const response =
            await fetch(
                "/api/capabilities/registry?" +
                params.toString()
            );

        const data =
            await response.json();

        if (!data.success) {
            throw new Error(
                data.error ||
                "Capability registry simulation failed."
            );
        }

        const rows =
            (data.capabilities || [])
                .map(item => `
                    <div class="experiment-result-row">
                        <strong>${item.node}</strong>
                        <span>${item.capability}</span>
                        <span>${item.state}</span>
                    </div>
                `)
                .join("");

        const discoveryNodes =
            data.discovery &&
            Array.isArray(data.discovery.found)
                ? data.discovery.found
                : [];

        const discoveryText =
            discoveryNodes.length > 0
                ? discoveryNodes.join(", ")
                : "No matching providers";

        result.innerHTML = `
            <div class="experiment-result-success">
                <strong>Simulation Result PASSED</strong>
                <p>
                    Registered capabilities: ${data.total}
                </p>
            </div>

            <div class="experiment-result-section">
                <strong>Applied Configuration</strong>
                <p>
                    Communication: ${communicationProviders}
                    &nbsp;|&nbsp;
                    Sensing: ${sensingProviders}
                    &nbsp;|&nbsp;
                    Computing: ${computingProviders}
                    &nbsp;|&nbsp;
                    AI: ${aiProviders}
                </p>
                <p>
                    Discovery Query: ${queryType}
                </p>
            </div>

            <div class="experiment-result-section">
                <strong>Registered Capabilities</strong>
                ${rows}
            </div>

            <div class="experiment-result-section">
                <strong>Discovery Result</strong>
                <p>
                    ${queryType} →
                    ${discoveryText}
                </p>
                <p>
                    Matching providers:
                    ${data.discovery
                        ? data.discovery.count
                        : discoveryNodes.length}
                </p>
            </div>
        `;

    } catch (error) {

        console.error(
            "Capability registry experiment error:",
            error
        );

        result.innerHTML = `
            <div class="empty-state">
                <strong>Simulation failed</strong>
                <p>${error.message}</p>
            </div>
        `;

    } finally {

        button.disabled = false;
        button.textContent = "View Registry →";
    }
}

