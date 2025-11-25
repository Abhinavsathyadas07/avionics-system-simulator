# Python Visualization Tools

This directory contains Python scripts for visualizing and analyzing telemetry data from the Avionics System Simulator.

## Installation

Install the required Python packages:

```bash
pip install -r requirements.txt
```

Or using a virtual environment:

```bash
python3 -m venv venv
source venv/bin/activate  # On Windows: venv\Scripts\activate
pip install -r requirements.txt
```

## Usage

### Visualize Telemetry Data

After running the simulator, visualize the flight data:

```bash
python visualize_telemetry.py ../logs/flight_data_YYYYMMDD_HHMMSS.csv
```

This script generates:
- Flight profile plots (altitude, airspeed, vertical speed)
- Control surface position charts
- Sensor data analysis
- Flight phase timeline
- Statistical summary

## Output

Visualizations are saved in the `plots/` directory and displayed interactively.

## Features

- **Flight Profile Analysis**: Comprehensive altitude, airspeed, and vertical speed charts
- **Control Surface Monitoring**: Track elevator, aileron, rudder, and throttle positions
- **Sensor Health**: Visualize sensor readings and fault periods
- **Phase Timeline**: Color-coded timeline of flight phases
- **Statistics**: Detailed flight statistics and performance metrics
