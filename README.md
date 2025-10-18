# Deadlock Detection and Prevention System

A desktop application built with C++ and Python (Tkinter) for simulating, detecting, and preventing deadlocks in operating systems.

## Features

- **Deadlock Detection**: Resource Allocation Graph (RAG) with cycle detection
- **Deadlock Prevention**: Banker's Algorithm for safe state verification
- **Visual Interface**: Real-time graph visualization of resource allocation
- **Process Management**: Create, manage, and monitor processes
- **Resource Management**: Allocate and deallocate system resources

## Prerequisites

- Ubuntu/Linux system
- g++ compiler (C++11 or later)
- Python 3.x
- Tkinter (python3-tk)
- Make

## Installation

1. **Install dependencies**:
```bash
sudo apt update
sudo apt install -y build-essential g++ make python3 python3-pip python3-tk
pip3 install matplotlib numpy
```

2. **Clone/Create project structure**:
```bash
mkdir -p deadlock-project/{cpp,python,logs}
cd deadlock-project
```

3. **Build C++ shared library**:
```bash
make
```

## Project Structure

```
deadlock-project/
├── README.md
├── Makefile
├── cpp/
│   ├── deadlock.h       # All C++ class definitions
│   ├── deadlock.cpp     # All C++ implementations
│   └── main.cpp         # Shared library interface
├── python/
│   ├── gui.py           # Complete Tkinter GUI
│   └── main.py          # Application entry + C++ bridge
└── logs/                # Runtime logs
```

## Usage

1. **Build the project**:
```bash
make
```

2. **Run the application**:
```bash
cd python
python3 main.py
```

## How It Works

### C++ Backend
- **Process & Resource Management**: Handles process states and resource allocation
- **Deadlock Detection**: Implements RAG and cycle detection algorithms
- **Banker's Algorithm**: Provides deadlock prevention through safe state checking

### Python Frontend
- **GUI Interface**: User-friendly Tkinter interface
- **Visualization**: Real-time graph display of resource allocation
- **C++ Integration**: Uses ctypes to call C++ functions from Python

## Algorithms Implemented

1. **Resource Allocation Graph (RAG)**: Visual representation of processes and resources
2. **Cycle Detection**: DFS-based algorithm to detect deadlock cycles
3. **Banker's Algorithm**: Prevents deadlock by checking safe sequences

## Testing

The application allows you to:
- Create multiple processes with resource requirements
- Allocate/deallocate resources dynamically
- Detect deadlocks in real-time
- Test banker's algorithm for safe resource allocation

## Troubleshooting

**Issue**: `libdeadlock.so: cannot open shared object file`
**Solution**: Make sure you've run `make` in the project root directory

**Issue**: `ImportError: No module named 'tkinter'`
**Solution**: Install tkinter with `sudo apt install python3-tk`

**Issue**: GUI doesn't display graphs
**Solution**: Install matplotlib with `pip3 install matplotlib`

## Author

Created for Operating Systems course project

## License

MIT License
