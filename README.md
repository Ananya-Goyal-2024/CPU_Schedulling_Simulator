# CPU Scheduling Simulator

This is a console-based C++ project that simulates various CPU scheduling algorithms used in operating systems. It allows users to select different scheduling algorithms and modes to visualize process execution, trace their states, and view performance statistics.

## 🚀 Features

- Supports multiple scheduling algorithms:
  - First Come First Serve (FCFS)
  - Round Robin (RR) with configurable time quantum
  - Shortest Process Next (SPN)
  - Shortest Remaining Time (SRT)
  - Priority Scheduling (Preemptive & Non-Preemptive with aging)
  
- Input modes:
  - Trace Mode
  - Stats Mode
  - Trace + Stats Mode

- JSON-based input file for easy configuration
- Modular and extensible code

---

## 📂 File Structure
pgsql
Copy
Edit
*  OSproject                // Executable (compiled binary)
  
*  OSproject.cpp            // Main C++ source file

*  son.hpp                  // JSON parser (nlohmann library)
  
*  nput.json                // Sample input file with process data
  
*  .vscode/settings.json    // VS Code configuration

  
🧪 Sample Input: input.json
json
Copy
Edit

{
   "algorithm": "RR",
  
   "quantum": 4,
  
   "mode": "Trace+Stats",
   
   "processes": [
   
     { "pid": 1, "arrival": 0, "burst": 5, "priority": 2 },
     
     { "pid": 2, "arrival": 2, "burst": 3, "priority": 1 },
     
     { "pid": 3, "arrival": 4, "burst": 8, "priority": 3 }
     
  ]
  
}


🛠️ How to Run

🧾 Compile the code:

bash
Copy
Edit

g++ OSproject.cpp -o OSproject

▶️ Run the executable:

bash
Copy
Edit
./OSproject

Follow the prompts to select algorithm and input mode.


📦 Dependencies

nlohmann/json.hpp — for parsing JSON input

👩‍💻 Author
Ananya Goyal
🔗 LinkedIn Profile

