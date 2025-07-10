#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <iomanip>
#include <fstream>
#include <sstream>
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

struct Process {
    string name;
    int arrival, burst, priority;
    int remaining, start = -1, finish = 0;
    int turnaround = 0, waiting = 0, response = -1;
    bool completed = false;
    int waited_time = 0; // for aging
};

void print_stats(const vector<Process>& processes) {
    cout << "\nProcess Name\tArrival Time\tBurst Time\tCompletion Time\tTurnaround Time\tWaiting Time\tResponse Time\n";
    for (const auto& p : processes) {
        cout << p.name << "\t\t" << p.arrival << "\t\t" << p.burst << "\t\t" << p.finish
             << "\t\t" << p.turnaround << "\t\t" << p.waiting << "\t\t" << p.response << "\n";
    }
}

void print_gantt_chart(const vector<string>& gantt) {
    cout << "\n--- Gantt Chart ---\n";
    for (const string& entry : gantt) cout << entry << " ";
    cout << "\n";
}

void fcfs(vector<Process> p, bool traceMode) {
    int time = 0;
    sort(p.begin(), p.end(), [](auto a, auto b) { return a.arrival < b.arrival; });
    vector<string> gantt;
    for (auto& proc : p) {
        time = max(time, proc.arrival);
        proc.start = time;
        for (int i = 0; i < proc.burst; i++) {
            if (traceMode) gantt.push_back(proc.name);
        }
        proc.finish = time + proc.burst;
        time = proc.finish;
        proc.turnaround = proc.finish - proc.arrival;
        proc.waiting = proc.turnaround - proc.burst;
        proc.response = proc.start - proc.arrival;
    }
    cout << "--- FCFS ---";
    print_stats(p);
    if (traceMode) print_gantt_chart(gantt);
}

void rr(vector<Process> p, int quantum, bool traceMode) {
    int time = 0, n = p.size(), done = 0;
    queue<int> q;
    vector<bool> inQueue(n, false);
    vector<string> gantt;
    for (int i = 0; i < n; ++i) p[i].remaining = p[i].burst;

    while (done < n) {
        for (int i = 0; i < n; ++i) {
            if (p[i].arrival <= time && !inQueue[i] && !p[i].completed) {
                q.push(i);
                inQueue[i] = true;
            }
        }

        if (q.empty()) {
            if (traceMode) gantt.push_back("Idle");
            time++;
            continue;
        }

        int idx = q.front();
        q.pop();
        inQueue[idx] = false;

        if (p[idx].response == -1)
            p[idx].response = time - p[idx].arrival;

        int slice = min(quantum, p[idx].remaining);
        for (int i = 0; i < slice; i++) {
            if (traceMode) gantt.push_back(p[idx].name);
            time++;
            p[idx].remaining--;

            for (int j = 0; j < n; ++j) {
                if (p[j].arrival == time && !inQueue[j] && !p[j].completed) {
                    q.push(j);
                    inQueue[j] = true;
                }
            }
        }

        if (p[idx].remaining > 0) {
            q.push(idx);
            inQueue[idx] = true;
        } else {
            p[idx].completed = true;
            p[idx].finish = time;
            p[idx].turnaround = time - p[idx].arrival;
            p[idx].waiting = p[idx].turnaround - p[idx].burst;
            done++;
        }
    }

    cout << "\n--- Round Robin (Q=" << quantum << ") ---";
    print_stats(p);
    if (traceMode) print_gantt_chart(gantt);
}


void spn(vector<Process> p, bool traceMode) {
    int time = 0, n = p.size(), done = 0;
    vector<string> gantt;
    while (done < n) {
        int idx = -1, min_bt = 1e9;
        for (int i = 0; i < n; ++i)
            if (!p[i].completed && p[i].arrival <= time && p[i].burst < min_bt)
                min_bt = p[i].burst, idx = i;
        if (idx == -1) {
            if (traceMode) gantt.push_back("Idle");
            time++;
            continue;
        }
        p[idx].start = time;
        p[idx].response = time - p[idx].arrival;
        for (int i = 0; i < p[idx].burst; i++) {
            if (traceMode) gantt.push_back(p[idx].name);
        }
        time += p[idx].burst;
        p[idx].finish = time;
        p[idx].turnaround = time - p[idx].arrival;
        p[idx].waiting = p[idx].turnaround - p[idx].burst;
        p[idx].completed = true;
        done++;
    }
    cout << "--- SPN ---";
    print_stats(p);
    if (traceMode) print_gantt_chart(gantt);
}

void srt(vector<Process> p, bool traceMode) {
    int time = 0, n = p.size(), done = 0;
    vector<string> gantt;
    for (int i = 0; i < n; ++i) p[i].remaining = p[i].burst;
    while (done < n) {
        int idx = -1, min_rt = 1e9;
        for (int i = 0; i < n; ++i)
            if (!p[i].completed && p[i].arrival <= time && p[i].remaining < min_rt && p[i].remaining > 0)
                min_rt = p[i].remaining, idx = i;
        if (idx == -1) {
            if (traceMode) gantt.push_back("Idle");
            time++;
            continue;
        }
        if (p[idx].response == -1)
            p[idx].response = time - p[idx].arrival;
        p[idx].remaining--;
        if (traceMode) gantt.push_back(p[idx].name);
        time++;
        if (p[idx].remaining == 0) {
            p[idx].completed = true;
            p[idx].finish = time;
            p[idx].turnaround = time - p[idx].arrival;
            p[idx].waiting = p[idx].turnaround - p[idx].burst;
            done++;
        }
    }
    cout << "--- SRT ---";
    print_stats(p);
    if (traceMode) print_gantt_chart(gantt);
}

void priority_np(vector<Process> p) {
    int time = 0, n = p.size(), done = 0;
    vector<string> gantt;
    while (done < n) {
        int idx = -1, high = 1e9;
        for (int i = 0; i < n; ++i)
            if (!p[i].completed && p[i].arrival <= time && p[i].priority < high)
                high = p[i].priority, idx = i;
        if (idx == -1) { time++; continue; }
        p[idx].start = time;
        p[idx].response = time - p[idx].arrival;
        for (int i = 0; i < p[idx].burst; i++) {
            gantt.push_back(p[idx].name);
        }
        time += p[idx].burst;
        p[idx].finish = time;
        p[idx].turnaround = time - p[idx].arrival;
        p[idx].waiting = p[idx].turnaround - p[idx].burst;
        p[idx].completed = true;
        done++;
    }
    cout << "--- Priority (Non-Preemptive) ---";
    print_stats(p);
    print_gantt_chart(gantt);
}
void priority_p(vector<Process> p, bool traceMode) {
    int time = 0, n = p.size(), done = 0;
    vector<string> gantt;
    const int aging_interval = 5;

    for (int i = 0; i < n; ++i) p[i].remaining = p[i].burst;
    while (done < n) {
        // Aging: Increase priority (lower number) for waiting processes
        for (int i = 0; i < n; ++i) {
            if (!p[i].completed && p[i].arrival <= time && p[i].remaining > 0 && i != -1) {
                p[i].waited_time++;
                if (p[i].waited_time % aging_interval == 0 && p[i].priority > 0) {
                    p[i].priority--;
                }
            }
        }

        int idx = -1, high = 1e9;
        for (int i = 0; i < n; ++i) {
            if (!p[i].completed && p[i].arrival <= time && p[i].priority < high && p[i].remaining > 0) {
                high = p[i].priority;
                idx = i;
            }
        }

        if (idx == -1) {
            if (traceMode) gantt.push_back("Idle");
            time++;
            continue;
        }

        // Reset waited_time for running process
        p[idx].waited_time = 0;

        if (p[idx].response == -1)
            p[idx].response = time - p[idx].arrival;

        p[idx].remaining--;
        if (traceMode) gantt.push_back(p[idx].name);
        time++;

        if (p[idx].remaining == 0) {
            p[idx].completed = true;
            p[idx].finish = time;
            p[idx].turnaround = time - p[idx].arrival;
            p[idx].waiting = p[idx].turnaround - p[idx].burst;
            done++;
        }
    }

    cout << "\n--- Priority (Preemptive with Aging) ---";
    print_stats(p);
    if (traceMode) print_gantt_chart(gantt);
}

vector<Process> get_input_from_json(const string& filename, bool has_priority) {
    ifstream inFile(filename);
    json data;
    inFile >> data;

    vector<Process> p;
    for (auto& item : data["processes"]) {
        Process proc;
        proc.name = item["name"];
        proc.arrival = item["arrival"];
        proc.burst = item["burst"];
        proc.priority = has_priority ? int(item["priority"]) : 0;
        p.push_back(proc);
    }
    return p;
}

vector<Process> get_input_from_console(int n, bool has_priority) {
    vector<Process> p(n);
    for (int i = 0; i < n; i++) {
        cout << "\nProcess " << i+1 << "\n";
        cout << "Name: "; cin >> p[i].name;
        cout << "Arrival Time: "; cin >> p[i].arrival;
        cout << "Burst Time: "; cin >> p[i].burst;
        if (has_priority) {
            cout << "Priority (lower is higher): "; cin >> p[i].priority;
        } else {
            p[i].priority = 0;
        }
    }
    return p;
}

int main() {
    int choice, mode, inputType;
    cout << "Select Scheduling Algorithm:\n";
    cout << "1. First Come First Serve (FCFS)\n";
    cout << "2. Round Robin (RR)\n";
    cout << "3. Shortest Process Next (SPN)\n";
    cout << "4. Shortest Remaining Time (SRT)\n";
    cout << "5. Priority Scheduling (Non-Preemptive)\n";
    cout << "6. Priority Scheduling (Preemptive with Aging)\n";
    cout << "Choice: ";
    cin >> choice;

    cout << "\nSelect Mode:\n1. Stats Mode\n2. Trace + Stats Mode\nMode: ";
    cin >> mode;
    bool traceMode = (mode == 2);

    cout << "\nSelect Input Method:\n1. Console Input\n2. JSON File Input\nChoice: ";
    cin >> inputType;

    int n, quantum;
    vector<Process> p;
    bool needs_priority = (choice == 5 || choice == 6);

    if (inputType == 1) {
        cout << "Enter number of processes: "; cin >> n;
        p = get_input_from_console(n, needs_priority);
    } else {
        string filename;
        cout << "Enter JSON filename (e.g. input.json): ";
        cin >> filename;
        p = get_input_from_json(filename, needs_priority);
    }

    if (choice == 2) {
        cout << "Enter Time Quantum: ";
        cin >> quantum;
    }

    switch(choice) {
        case 1: fcfs(p, traceMode); break;
        case 2: rr(p, quantum, traceMode); break;
        case 3: spn(p, traceMode); break;
        case 4: srt(p, traceMode); break;
        case 5: priority_np(p); break;
        case 6: priority_p(p, traceMode); break;
        default: cout << "Invalid Choice\n";
    }

    return 0;
}
