# CPU Scheduling Algorithm Simulator

A C-based project that simulates different CPU scheduling algorithms and compares their performance.

## Project Overview

The CPU Scheduling Algorithm Simulator demonstrates how an operating system schedules processes for CPU execution. It calculates scheduling metrics and displays the execution order using a Gantt chart.

## Algorithms Implemented

1. **First Come First Serve (FCFS)**
2. **Shortest Job First (SJF)** – Non-preemptive
3. **Priority Scheduling** – Non-preemptive
4. **Round Robin (RR)** – Configurable time quantum

## Features

- Accepts process details such as arrival time, burst time and priority.
- Calculates completion time, waiting time and turnaround time.
- Displays a Gantt chart for process execution.
- Calculates average waiting time and average turnaround time.
- Allows comparison of all four scheduling algorithms.

## Technologies Used

- Programming Language: C
- Compiler: GCC
- Development/Execution Environment: OnlineGDB

## How to Run

Compile the source file:

```bash
gcc cpu_scheduling_simulator.c -o cpu_scheduler
```

Run the program:

```bash
./cpu_scheduler
```

On Windows, run:

```bash
cpu_scheduler.exe
```

## Project Files

- `cpu_scheduling_simulator.c` – Main C source code
- `CPU_Scheduling_Report.pdf` – Project documentation

## Team Members

1. Sachi Riya
2. Sadat Ahmad
3. Sandeep Kumar Choudhary
4. Satyam Singh Rathaur
5. Shumaiya Shamim

## Subject

Operating Systems and Shell Programming

---

**Project:** CPU Scheduling Algorithm Simulator
