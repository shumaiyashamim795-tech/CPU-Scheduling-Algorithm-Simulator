/*
 * CPU Scheduling Algorithm Simulator
 * Project: Operating Systems and Shell Programming
 * Language: C
 *
 * Algorithms:
 * 1. First Come First Served (FCFS)
 * 2. Shortest Job First (SJF - Non-preemptive)
 * 3. Priority Scheduling (Non-preemptive; smaller number = higher priority)
 * 4. Round Robin (preemptive; configurable time quantum)
 *
 * Tie-breaking:
 * - Earlier arrival time is preferred.
 * - If arrival times are equal, original input order is preferred.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PROCESSES 50

typedef struct {
    int pid;
    int arrival;
    int burst;
    int priority;
    int completion;
    int turnaround;
    int waiting;
    int remaining;
    int input_order;
} Process;

typedef struct {
    int pid;       /* 0 represents CPU idle */
    int start;
    int end;
} Segment;

void copy_processes(Process dest[], const Process src[], int n) {
    for (int i = 0; i < n; i++) {
        dest[i] = src[i];
        dest[i].completion = 0;
        dest[i].turnaround = 0;
        dest[i].waiting = 0;
        dest[i].remaining = src[i].burst;
    }
}

void calculate_metrics(Process p[], int n, double *avg_wait, double *avg_tat) {
    double total_wait = 0.0, total_tat = 0.0;
    for (int i = 0; i < n; i++) {
        p[i].turnaround = p[i].completion - p[i].arrival;
        p[i].waiting = p[i].turnaround - p[i].burst;
        total_wait += p[i].waiting;
        total_tat += p[i].turnaround;
    }
    *avg_wait = total_wait / n;
    *avg_tat = total_tat / n;
}

void print_results(const char *title, Process p[], int n,
                   Segment gantt[], int gcount) {
    double avg_wait, avg_tat;
    calculate_metrics(p, n, &avg_wait, &avg_tat);

    printf("\n========== %s ==========\n", title);
    printf("%-8s %-8s %-8s %-10s %-12s %-10s\n",
           "Process", "Arrival", "Burst", "Priority", "Waiting", "Turnaround");
    for (int i = 0; i < n; i++) {
        printf("P%-7d %-8d %-8d %-10d %-12d %-10d\n",
               p[i].pid, p[i].arrival, p[i].burst, p[i].priority,
               p[i].waiting, p[i].turnaround);
    }
    printf("Average Waiting Time    : %.2f\n", avg_wait);
    printf("Average Turnaround Time : %.2f\n", avg_tat);

    if (gcount > 0) {
        printf("Gantt Chart:\n");
        for (int i = 0; i < gcount; i++) {
            if (gantt[i].pid == 0)
                printf("| IDLE ");
            else
                printf("|  P%d  ", gantt[i].pid);
        }
        printf("|\n%d", gantt[0].start);
        for (int i = 0; i < gcount; i++)
            printf("%7d", gantt[i].end);
        printf("\n");
    }
}

void add_segment(Segment gantt[], int *count, int pid, int start, int end) {
    if (end <= start) return;
    if (*count > 0 && gantt[*count - 1].pid == pid &&
        gantt[*count - 1].end == start) {
        gantt[*count - 1].end = end;
        return;
    }
    gantt[*count].pid = pid;
    gantt[*count].start = start;
    gantt[*count].end = end;
    (*count)++;
}

int select_available(Process p[], int n, int time, int done[],
                     int mode) {
    int best = -1;
    for (int i = 0; i < n; i++) {
        if (done[i] || p[i].arrival > time) continue;
        if (best == -1) {
            best = i;
            continue;
        }

        int better = 0;
        if (mode == 1) { /* SJF */
            if (p[i].burst < p[best].burst) better = 1;
            else if (p[i].burst == p[best].burst &&
                     p[i].arrival < p[best].arrival) better = 1;
        } else { /* Priority: lower numeric value means higher priority */
            if (p[i].priority < p[best].priority) better = 1;
            else if (p[i].priority == p[best].priority &&
                     p[i].arrival < p[best].arrival) better = 1;
        }
        if (better) best = i;
    }
    return best;
}

void run_non_preemptive(const Process original[], int n, int mode,
                        const char *title) {
    Process p[MAX_PROCESSES];
    int done[MAX_PROCESSES] = {0};
    Segment gantt[MAX_PROCESSES * 2];
    int gcount = 0, completed = 0, time = 0;

    copy_processes(p, original, n);

    while (completed < n) {
        int idx = select_available(p, n, time, done, mode);

        if (idx == -1) {
            int next_arrival = 2147483647;
            for (int i = 0; i < n; i++)
                if (!done[i] && p[i].arrival < next_arrival)
                    next_arrival = p[i].arrival;
            add_segment(gantt, &gcount, 0, time, next_arrival);
            time = next_arrival;
            continue;
        }

        int start = time;
        time += p[idx].burst;
        p[idx].completion = time;
        done[idx] = 1;
        completed++;
        add_segment(gantt, &gcount, p[idx].pid, start, time);
    }

    print_results(title, p, n, gantt, gcount);
}

void run_fcfs(const Process original[], int n) {
    Process p[MAX_PROCESSES];
    Segment gantt[MAX_PROCESSES * 2];
    int gcount = 0, time = 0;

    copy_processes(p, original, n);

    /* Stable selection by arrival time, then original input order. */
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            if (p[j].arrival < p[i].arrival ||
                (p[j].arrival == p[i].arrival &&
                 p[j].input_order < p[i].input_order)) {
                Process temp = p[i]; p[i] = p[j]; p[j] = temp;
            }
        }
    }

    for (int i = 0; i < n; i++) {
        if (time < p[i].arrival) {
            add_segment(gantt, &gcount, 0, time, p[i].arrival);
            time = p[i].arrival;
        }
        int start = time;
        time += p[i].burst;
        p[i].completion = time;
        add_segment(gantt, &gcount, p[i].pid, start, time);
    }

    print_results("FCFS Scheduling", p, n, gantt, gcount);
}

void run_round_robin(const Process original[], int n, int quantum) {
    Process p[MAX_PROCESSES];
    Segment gantt[MAX_PROCESSES * 100];
    int queue[MAX_PROCESSES * 100];
    int front = 0, rear = 0, gcount = 0;
    int time = 0, completed = 0, next = 0;

    copy_processes(p, original, n);

    /* Sort process indices by arrival time, preserving input order on ties. */
    int order[MAX_PROCESSES];
    for (int i = 0; i < n; i++) order[i] = i;
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            if (p[order[j]].arrival < p[order[i]].arrival ||
                (p[order[j]].arrival == p[order[i]].arrival &&
                 p[order[j]].input_order < p[order[i]].input_order)) {
                int temp = order[i]; order[i] = order[j]; order[j] = temp;
            }
        }
    }

    while (completed < n) {
        if (front == rear && next < n && time < p[order[next]].arrival)
            time = p[order[next]].arrival;

        while (next < n && p[order[next]].arrival <= time)
            queue[rear++] = order[next++];

        if (front == rear) continue;

        int idx = queue[front++];
        int start = time;
        int run_for = (p[idx].remaining < quantum) ?
                      p[idx].remaining : quantum;
        time += run_for;
        p[idx].remaining -= run_for;
        add_segment(gantt, &gcount, p[idx].pid, start, time);

        /* Add processes that arrived during this time slice. */
        while (next < n && p[order[next]].arrival <= time)
            queue[rear++] = order[next++];

        if (p[idx].remaining > 0) {
            queue[rear++] = idx;
        } else {
            p[idx].completion = time;
            completed++;
        }
    }

    print_results("Round Robin Scheduling", p, n, gantt, gcount);
}

int main(void) {
    Process processes[MAX_PROCESSES];
    int n, choice, quantum;

    printf("============================================\n");
    printf("       CPU SCHEDULING ALGORITHM SIMULATOR\n");
    printf("============================================\n");

    do {
        printf("Enter number of processes (1-%d): ", MAX_PROCESSES);
        if (scanf("%d", &n) != 1) return 1;
        if (n < 1 || n > MAX_PROCESSES)
            printf("Invalid number. Please try again.\n");
    } while (n < 1 || n > MAX_PROCESSES);

    for (int i = 0; i < n; i++) {
        processes[i].pid = i + 1;
        processes[i].input_order = i;
        printf("\nEnter Arrival Time, Burst Time and Priority for P%d: ", i + 1);
        if (scanf("%d %d %d", &processes[i].arrival,
                  &processes[i].burst, &processes[i].priority) != 3)
            return 1;

        if (processes[i].arrival < 0 || processes[i].burst <= 0 ||
            processes[i].priority < 0) {
            printf("Invalid input. Arrival/priority must be non-negative "
                   "and burst time must be positive.\n");
            return 1;
        }
    }

    do {
        printf("\n--------------- MENU ---------------\n");
        printf("1. FCFS Scheduling\n");
        printf("2. SJF (Non-preemptive)\n");
        printf("3. Priority (Non-preemptive)\n");
        printf("4. Round Robin\n");
        printf("5. Run all algorithms\n");
        printf("0. Exit\n");
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1) return 1;

        switch (choice) {
            case 1:
                run_fcfs(processes, n);
                break;
            case 2:
                run_non_preemptive(processes, n, 1, "SJF (Non-preemptive)");
                break;
            case 3:
                run_non_preemptive(processes, n, 2,
                                   "Priority (Non-preemptive)");
                break;
            case 4:
                printf("Enter time quantum: ");
                if (scanf("%d", &quantum) != 1) return 1;
                if (quantum <= 0) {
                    printf("Time quantum must be positive.\n");
                    break;
                }
                run_round_robin(processes, n, quantum);
                break;
            case 5:
                run_fcfs(processes, n);
                run_non_preemptive(processes, n, 1,
                                   "SJF (Non-preemptive)");
                run_non_preemptive(processes, n, 2,
                                   "Priority (Non-preemptive)");
                printf("\nFor Round Robin, enter a time quantum: ");
                if (scanf("%d", &quantum) != 1) return 1;
                if (quantum > 0) run_round_robin(processes, n, quantum);
                else printf("Invalid quantum; Round Robin skipped.\n");
                break;
            case 0:
                printf("Thank you for using the simulator.\n");
                break;
            default:
                printf("Invalid menu choice. Please try again.\n");
        }
    } while (choice != 0);

    return 0;
}
