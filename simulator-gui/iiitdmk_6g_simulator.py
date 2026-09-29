import os
import subprocess
import tkinter as tk
from tkinter import ttk, messagebox
from threading import Thread


# ============================================================
# IIITDMK 6G SIMULATOR
# GUI foundation + real ns-3 executable integration
# ============================================================

PROJECT_ROOT = os.path.abspath(
    os.path.join(os.path.dirname(__file__), "..")
)


SIMULATIONS = {
    "Basic 6G Network": {
        "executable": "build/contrib/6g-lena/network-nodes/examples/ns3.48-6g-basic-network-example-default",
        "parameters": {},
    },

    "Communication + Computing": {
        "executable": "build/contrib/6g-lena/network-nodes/examples/ns3.48-6g-communication-computing-example-default",
        "parameters": {},
    },

    "Heterogeneous Multi-Node Network": {
        "executable": "build/contrib/6g-lena/network-nodes/examples/ns3.48-6g-heterogeneous-multi-node-example-default",
        "parameters": {},
    },

    "Service Placement": {
        "executable": "build/contrib/6g-lena/network-nodes/examples/ns3.48-6g-service-placement-example-default",
        "parameters": {},
    },

    "Mobility / Handover": {
        "executable": "build/contrib/6g-lena/network-nodes/examples/ns3.48-6g-mobility-handover-example-default",
        "parameters": {},
    },

    "Dynamic Resource State": {
        "executable": "build/contrib/6g-lena/network-nodes/examples/ns3.48-6g-dynamic-resource-state-example-default",
        "parameters": {},
    },

    "End-to-End Session": {
        "executable": "build/contrib/6g-lena/network-nodes/examples/ns3.48-6g-end-to-end-session-experiment-default",
        "parameters": {},
    },

    "Communication + Computing Load": {
        "executable": "build/contrib/6g-lena/network-nodes/examples/ns3.48-6g-communication-computing-load-experiment-default",
        "parameters": {},
    },

    "Heterogeneous Network": {
        "executable": "build/contrib/6g-lena/network-nodes/examples/ns3.48-6g-heterogeneous-network-experiment-default",
        "parameters": {},
    },

    "Mobility / Handover Experiment": {
        "executable": "build/contrib/6g-lena/network-nodes/examples/ns3.48-6g-mobility-handover-experiment-default",
        "parameters": {},
    },

    "Dynamic Capability Discovery": {
        "executable": "build/contrib/6g-lena/6g-core/examples/ns3.48-6g-capability-experiment-default",
        "parameters": {
            "requests": {
                "label": "Number of Requests",
                "type": "int",
                "default": 10,
            },
            "profile": {
                "label": "Workload Profile",
                "type": "choice",
                "values": ["1", "2"],
                "default": "1",
            },
            "updates": {
                "label": "Runtime Resource Updates",
                "type": "choice",
                "values": ["ON", "OFF"],
                "default": "ON",
            },
        },
    },

    "Constraint-Aware Capability Discovery": {
        "executable": "build/contrib/6g-lena/6g-core/examples/ns3.48-6g-capability-constraint-experiment-default",
        "parameters": {},
    },

    "Capability Freshness": {
        "executable": "build/contrib/6g-lena/6g-core/examples/ns3.48-6g-capability-freshness-experiment-default",
        "parameters": {},
    },

    "Capability Update": {
        "executable": "build/contrib/6g-lena/6g-core/examples/ns3.48-6g-capability-update-experiment-default",
        "parameters": {},
    },

    "Capability Scalability": {
        "executable": "build/contrib/6g-lena/6g-core/examples/ns3.48-6g-capability-scalability-experiment-default",
        "parameters": {
            "repetitions": {
                "label": "Discovery Repetitions",
                "type": "int",
                "default": 1000,
            },
        },
    },

    "Resource Stress": {
        "executable": "build/contrib/6g-lena/6g-core/examples/ns3.48-6g-resource-stress-experiment-default",
        "parameters": {},
    },

    "Service Placement Experiment": {
        "executable": "build/contrib/6g-lena/6g-core/examples/ns3.48-6g-service-placement-experiment-default",
        "parameters": {
            "repetitions": {
                "label": "Service Placement Requests",
                "type": "int",
                "default": 5,
            },
        },
    },

    "Core Example": {
        "executable": "build/contrib/6g-lena/6g-core/examples/ns3.48-6g-core-example-default",
        "parameters": {},
    },

    "Freshness Example": {
        "executable": "build/contrib/6g-lena/6g-core/examples/ns3.48-6g-freshness-experiment-default",
        "parameters": {},
    },

    "6G Device Example": {
        "executable": "build/contrib/6g-lena/6g-devices/examples/ns3.48-6g-device-example-default",
        "parameters": {},
    },

    "Capability Registry Example": {
        "executable": "build/contrib/6g-lena/6g-capabilities/examples/ns3.48-6g-capability-registry-example-default",
        "parameters": {},
    },

    "Network Nodes Example": {
        "executable": "build/contrib/6g-lena/network-nodes/examples/ns3.48-network-nodes-example-default",
        "parameters": {},
    },
}


class IIITDMKSimulator:
    def __init__(self, root):
        self.root = root
        self.root.title("IIITDMK 6G SIMULATOR")
        self.root.geometry("1100x700")
        self.root.minsize(900, 600)

        self.parameter_widgets = {}

        self.build_interface()
        self.update_configuration_panel()

    # --------------------------------------------------------
    # Main interface
    # --------------------------------------------------------

    def build_interface(self):
        header = tk.Frame(self.root, padx=20, pady=15)
        header.pack(fill="x")

        title = tk.Label(
            header,
            text="IIITDMK 6G SIMULATOR",
            font=("Arial", 24, "bold"),
        )
        title.pack(anchor="w")

        subtitle = tk.Label(
            header,
            text="6G Network Simulation Platform",
            font=("Arial", 11),
        )
        subtitle.pack(anchor="w", pady=(3, 0))

        separator = ttk.Separator(self.root, orient="horizontal")
        separator.pack(fill="x")

        main = tk.Frame(self.root, padx=20, pady=15)
        main.pack(fill="both", expand=True)

        # ----------------------------------------------------
        # Simulation selection
        # ----------------------------------------------------

        selection_frame = ttk.LabelFrame(
            main,
            text="Simulation Example",
            padding=15,
        )
        selection_frame.pack(fill="x", pady=(0, 12))

        self.simulation_var = tk.StringVar()

        self.simulation_combo = ttk.Combobox(
            selection_frame,
            textvariable=self.simulation_var,
            values=list(SIMULATIONS.keys()),
            state="readonly",
            width=60,
        )
        self.simulation_combo.pack(fill="x")
        self.simulation_combo.bind(
            "<<ComboboxSelected>>",
            lambda event: self.update_configuration_panel(),
        )

        self.simulation_combo.current(0)

        # ----------------------------------------------------
        # Configuration
        # ----------------------------------------------------

        self.configuration_frame = ttk.LabelFrame(
            main,
            text="Configuration",
            padding=15,
        )
        self.configuration_frame.pack(fill="x", pady=(0, 12))

        # ----------------------------------------------------
        # Run controls
        # ----------------------------------------------------

        controls = tk.Frame(main)
        controls.pack(fill="x", pady=(0, 12))

        self.run_button = ttk.Button(
            controls,
            text="▶  RUN SIMULATION",
            command=self.run_simulation,
        )
        self.run_button.pack(side="left")

        self.status_var = tk.StringVar(value="Ready")
        status_label = tk.Label(
            controls,
            textvariable=self.status_var,
            font=("Arial", 10, "bold"),
        )
        status_label.pack(side="left", padx=20)

        # ----------------------------------------------------
        # Output
        # ----------------------------------------------------

        output_frame = ttk.LabelFrame(
            main,
            text="Simulation Output",
            padding=10,
        )
        output_frame.pack(fill="both", expand=True)

        self.output_text = tk.Text(
            output_frame,
            wrap="none",
            font=("Courier New", 10),
        )
        self.output_text.pack(
            side="left",
            fill="both",
            expand=True,
        )

        scrollbar = ttk.Scrollbar(
            output_frame,
            orient="vertical",
            command=self.output_text.yview,
        )
        scrollbar.pack(side="right", fill="y")

        self.output_text.configure(
            yscrollcommand=scrollbar.set
        )

    # --------------------------------------------------------
    # Configuration panel
    # --------------------------------------------------------

    def update_configuration_panel(self):
        for widget in self.configuration_frame.winfo_children():
            widget.destroy()

        self.parameter_widgets.clear()

        name = self.simulation_var.get()
        simulation = SIMULATIONS[name]
        parameters = simulation["parameters"]

        if not parameters:
            label = tk.Label(
                self.configuration_frame,
                text="This simulation uses its built-in/default configuration.",
                font=("Arial", 10),
            )
            label.pack(anchor="w")
            return

        for key, config in parameters.items():
            row = tk.Frame(self.configuration_frame)
            row.pack(fill="x", pady=5)

            label = tk.Label(
                row,
                text=config["label"],
                width=30,
                anchor="w",
            )
            label.pack(side="left")

            if config["type"] == "choice":
                variable = tk.StringVar(
                    value=config["default"]
                )

                widget = ttk.Combobox(
                    row,
                    textvariable=variable,
                    values=config["values"],
                    state="readonly",
                    width=20,
                )
                widget.pack(side="left")

            else:
                variable = tk.StringVar(
                    value=str(config["default"])
                )

                widget = ttk.Entry(
                    row,
                    textvariable=variable,
                    width=22,
                )
                widget.pack(side="left")

            self.parameter_widgets[key] = variable

    # --------------------------------------------------------
    # Build command
    # --------------------------------------------------------

    def build_command(self):
        name = self.simulation_var.get()
        simulation = SIMULATIONS[name]

        executable = os.path.join(
            PROJECT_ROOT,
            simulation["executable"],
        )

        command = [executable]

        for key, config in simulation["parameters"].items():
            value = self.parameter_widgets[key].get()

            if config["type"] == "int":
                try:
                    value = int(value)
                    if value <= 0:
                        raise ValueError
                except ValueError:
                    raise ValueError(
                        f"{config['label']} must be a positive integer."
                    )

            if key == "updates":
                value = "1" if value == "ON" else "0"

            command.append(f"--{key}={value}")

        return command

    # --------------------------------------------------------
    # Run simulation
    # --------------------------------------------------------

    def run_simulation(self):
        try:
            command = self.build_command()
        except ValueError as error:
            messagebox.showerror(
                "Invalid Parameter",
                str(error),
            )
            return

        executable = command[0]

        if not os.path.isfile(executable):
            messagebox.showerror(
                "Executable Not Found",
                f"Simulation executable was not found:\n\n{executable}",
            )
            return

        self.run_button.configure(state="disabled")
        self.status_var.set("Running...")

        self.output_text.delete("1.0", tk.END)

        self.output_text.insert(
            tk.END,
            "IIITDMK 6G SIMULATOR\n"
            "=====================\n\n"
            f"Simulation: {self.simulation_var.get()}\n\n"
            f"Command:\n{' '.join(command)}\n\n"
            "----------------------------------------\n"
            "SIMULATION OUTPUT\n"
            "----------------------------------------\n\n",
        )

        thread = Thread(
            target=self.execute_simulation,
            args=(command,),
            daemon=True,
        )
        thread.start()

    # --------------------------------------------------------
    # Execute real ns-3 program
    # --------------------------------------------------------

    def execute_simulation(self, command):
        try:
            process = subprocess.Popen(
                command,
                cwd=PROJECT_ROOT,
                stdout=subprocess.PIPE,
                stderr=subprocess.STDOUT,
                text=True,
                bufsize=1,
            )

            for line in process.stdout:
                self.root.after(
                    0,
                    self.append_output,
                    line,
                )

            return_code = process.wait()

            self.root.after(
                0,
                self.simulation_finished,
                return_code,
            )

        except Exception as error:
            self.root.after(
                0,
                self.simulation_error,
                str(error),
            )

    # --------------------------------------------------------
    # GUI output helpers
    # --------------------------------------------------------

    def append_output(self, text):
        self.output_text.insert(tk.END, text)
        self.output_text.see(tk.END)

    def simulation_finished(self, return_code):
        if return_code == 0:
            self.status_var.set("Simulation completed")
            self.append_output(
                "\n----------------------------------------\n"
                "SIMULATION COMPLETED SUCCESSFULLY\n"
                "----------------------------------------\n"
            )
        else:
            self.status_var.set("Simulation failed")
            self.append_output(
                "\n----------------------------------------\n"
                f"SIMULATION FAILED (exit code {return_code})\n"
                "----------------------------------------\n"
            )

        self.run_button.configure(state="normal")

    def simulation_error(self, error):
        self.status_var.set("Error")
        self.append_output(
            "\n----------------------------------------\n"
            "SIMULATION ERROR\n"
            "----------------------------------------\n"
            f"{error}\n"
        )
        self.run_button.configure(state="normal")


def main():
    root = tk.Tk()
    IIITDMKSimulator(root)
    root.mainloop()


if __name__ == "__main__":
    main()
