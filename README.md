# Avionics System Simulator

An embedded avionics system simulator demonstrating real-time control principles for flight management systems.

## Overview

This project simulates a safety-critical avionics system with real-time sensor processing, flight control state management, fault handling, and telemetry visualization.

## Features

- **Flight Control System State Machine**: Models flight phases (takeoff, cruise, landing, emergency)
- **Sensor Data Processing**: Simulates altitude, airspeed, and pressure sensors with realistic data
- **Real-time System Monitoring**: Periodic state updates and sensor readings
- **Safety-critical Fault Handling**: Detects and responds to sensor failures and system faults
- **System Logging and Telemetry**: Comprehensive logging for post-flight analysis
- **Python Visualization**: Real-time and post-flight data visualization

## Tech Stack

- **C++**: Core embedded simulation engine
- **Python**: Data visualization and telemetry analysis
- **CMake**: Build system

## Project Structure

```
avionics-system-simulator/
├── src/               # C++ source files
├── include/           # C++ header files
├── python/            # Python visualization scripts
├── logs/              # System logs and telemetry data
├── tests/             # Unit tests
├── docs/              # Documentation
├── CMakeLists.txt     # CMake build configuration
└── README.md          # This file
```

## Getting Started

### Prerequisites

- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- CMake 3.15+
- Python 3.8+
- Python packages: matplotlib, numpy, pandas

### Building the Project

```bash
mkdir build
cd build
cmake ..
make
```

### Running the Simulator

```bash
./build/avionics_simulator
```

### Visualizing Telemetry Data

```bash
python python/visualize_telemetry.py logs/flight_data.csv
```

## Development Roadmap

- [x] Project structure setup
- [ ] Flight control state machine implementation
- [ ] Sensor data processing module
- [ ] Real-time monitoring system
- [ ] Fault detection and handling
- [ ] Telemetry logging system
- [ ] Python visualization tools
- [ ] Unit tests
- [ ] Documentation

## License

MIT License

## Author

Abhinav Sathya Das
