#!/usr/bin/env python3
"""
Avionics System Simulator - Telemetry Visualization

This script reads telemetry data from CSV files and generates
comprehensive visualizations for post-flight analysis.
"""

import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import sys
import os
from datetime import datetime

def load_telemetry(filename):
    """Load telemetry data from CSV file."""
    try:
        df = pd.read_csv(filename)
        print(f"Loaded {len(df)} data points from {filename}")
        return df
    except FileNotFoundError:
        print(f"Error: File '{filename}' not found")
        sys.exit(1)
    except Exception as e:
        print(f"Error loading file: {e}")
        sys.exit(1)

def plot_flight_profile(df):
    """Create flight profile visualization."""
    fig, axes = plt.subplots(3, 1, figsize=(14, 10))
    fig.suptitle('Flight Profile Analysis', fontsize=16, fontweight='bold')
    
    # Altitude profile
    axes[0].plot(df['SimulationTime'], df['Altitude'], 'b-', linewidth=2, label='Altitude')
    axes[0].set_ylabel('Altitude (m)', fontsize=12)
    axes[0].set_title('Altitude Profile')
    axes[0].grid(True, alpha=0.3)
    axes[0].legend()
    
    # Add flight phase markers
    phases = df['FlightPhase'].unique()
    colors = plt.cm.Set3(np.linspace(0, 1, len(phases)))
    for i, phase in enumerate(phases):
        phase_data = df[df['FlightPhase'] == phase]
        if not phase_data.empty:
            axes[0].axvspan(phase_data['SimulationTime'].iloc[0], 
                          phase_data['SimulationTime'].iloc[-1],
                          alpha=0.2, color=colors[i], label=phase)
    
    # Airspeed profile
    axes[1].plot(df['SimulationTime'], df['Airspeed'], 'r-', linewidth=2, label='Airspeed')
    axes[1].set_ylabel('Airspeed (m/s)', fontsize=12)
    axes[1].set_title('Airspeed Profile')
    axes[1].grid(True, alpha=0.3)
    axes[1].legend()
    
    # Vertical speed profile
    axes[2].plot(df['SimulationTime'], df['VerticalSpeed'], 'g-', linewidth=2, label='Vertical Speed')
    axes[2].axhline(y=0, color='k', linestyle='--', alpha=0.5)
    axes[2].set_xlabel('Simulation Time (s)', fontsize=12)
    axes[2].set_ylabel('Vertical Speed (m/s)', fontsize=12)
    axes[2].set_title('Vertical Speed Profile')
    axes[2].grid(True, alpha=0.3)
    axes[2].legend()
    
    plt.tight_layout()
    return fig

def plot_control_surfaces(df):
    """Visualize control surface positions."""
    fig, axes = plt.subplots(2, 2, figsize=(14, 10))
    fig.suptitle('Control Surface Analysis', fontsize=16, fontweight='bold')
    
    # Elevator
    axes[0, 0].plot(df['SimulationTime'], df['Elevator'], 'b-', linewidth=2)
    axes[0, 0].set_ylabel('Elevator Position', fontsize=11)
    axes[0, 0].set_title('Elevator Control')
    axes[0, 0].axhline(y=0, color='k', linestyle='--', alpha=0.5)
    axes[0, 0].grid(True, alpha=0.3)
    axes[0, 0].set_ylim(-1.1, 1.1)
    
    # Aileron
    axes[0, 1].plot(df['SimulationTime'], df['Aileron'], 'r-', linewidth=2)
    axes[0, 1].set_ylabel('Aileron Position', fontsize=11)
    axes[0, 1].set_title('Aileron Control')
    axes[0, 1].axhline(y=0, color='k', linestyle='--', alpha=0.5)
    axes[0, 1].grid(True, alpha=0.3)
    axes[0, 1].set_ylim(-1.1, 1.1)
    
    # Rudder
    axes[1, 0].plot(df['SimulationTime'], df['Rudder'], 'g-', linewidth=2)
    axes[1, 0].set_xlabel('Simulation Time (s)', fontsize=11)
    axes[1, 0].set_ylabel('Rudder Position', fontsize=11)
    axes[1, 0].set_title('Rudder Control')
    axes[1, 0].axhline(y=0, color='k', linestyle='--', alpha=0.5)
    axes[1, 0].grid(True, alpha=0.3)
    axes[1, 0].set_ylim(-1.1, 1.1)
    
    # Throttle
    axes[1, 1].plot(df['SimulationTime'], df['Throttle'], 'm-', linewidth=2)
    axes[1, 1].set_xlabel('Simulation Time (s)', fontsize=11)
    axes[1, 1].set_ylabel('Throttle Position', fontsize=11)
    axes[1, 1].set_title('Throttle Control')
    axes[1, 1].grid(True, alpha=0.3)
    axes[1, 1].set_ylim(-0.1, 1.1)
    
    plt.tight_layout()
    return fig

def plot_sensor_data(df):
    """Visualize sensor readings."""
    fig, axes = plt.subplots(2, 2, figsize=(14, 10))
    fig.suptitle('Sensor Data Analysis', fontsize=16, fontweight='bold')
    
    # Altitude sensor
    axes[0, 0].plot(df['SimulationTime'], df['Altitude'], 'b-', linewidth=1.5)
    axes[0, 0].set_ylabel('Altitude (m)', fontsize=11)
    axes[0, 0].set_title('Altitude Sensor')
    axes[0, 0].grid(True, alpha=0.3)
    
    # Pressure sensor
    axes[0, 1].plot(df['SimulationTime'], df['Pressure'], 'r-', linewidth=1.5)
    axes[0, 1].set_ylabel('Pressure (hPa)', fontsize=11)
    axes[0, 1].set_title('Pressure Sensor')
    axes[0, 1].grid(True, alpha=0.3)
    
    # Temperature sensor
    axes[1, 0].plot(df['SimulationTime'], df['Temperature'], 'g-', linewidth=1.5)
    axes[1, 0].set_xlabel('Simulation Time (s)', fontsize=11)
    axes[1, 0].set_ylabel('Temperature (°C)', fontsize=11)
    axes[1, 0].set_title('Temperature Sensor')
    axes[1, 0].grid(True, alpha=0.3)
    
    # Fault indicator
    axes[1, 1].plot(df['SimulationTime'], df['ActiveFaults'], 'k-', linewidth=2)
    axes[1, 1].set_xlabel('Simulation Time (s)', fontsize=11)
    axes[1, 1].set_ylabel('Active Faults', fontsize=11)
    axes[1, 1].set_title('System Faults Over Time')
    axes[1, 1].grid(True, alpha=0.3)
    axes[1, 1].fill_between(df['SimulationTime'], 0, df['ActiveFaults'], 
                            alpha=0.3, color='red', label='Fault Periods')
    
    plt.tight_layout()
    return fig

def plot_phase_timeline(df):
    """Create flight phase timeline."""
    fig, ax = plt.subplots(figsize=(14, 4))
    
    phases = df['FlightPhase'].unique()
    phase_colors = {
        'PREFLIGHT': '#gray',
        'TAKEOFF': '#ff6b6b',
        'CLIMB': '#4ecdc4',
        'CRUISE': '#45b7d1',
        'DESCENT': '#f9ca24',
        'APPROACH': '#f0932b',
        'LANDING': '#6ab04c',
        'EMERGENCY': '#eb4d4b'
    }
    
    for phase in phases:
        phase_data = df[df['FlightPhase'] == phase]
        if not phase_data.empty:
            start_time = phase_data['SimulationTime'].iloc[0]
            end_time = phase_data['SimulationTime'].iloc[-1]
            duration = end_time - start_time
            
            color = phase_colors.get(phase, '#95a5a6')
            ax.barh(0, duration, left=start_time, height=0.8, 
                   color=color, label=phase, edgecolor='black', linewidth=1.5)
            
            # Add text label
            if duration > 5:  # Only show label if phase is long enough
                ax.text(start_time + duration/2, 0, phase, 
                       ha='center', va='center', fontweight='bold', fontsize=10)
    
    ax.set_xlabel('Simulation Time (s)', fontsize=12)
    ax.set_yticks([])
    ax.set_title('Flight Phase Timeline', fontsize=14, fontweight='bold')
    ax.set_xlim(df['SimulationTime'].min(), df['SimulationTime'].max())
    ax.grid(True, axis='x', alpha=0.3)
    
    plt.tight_layout()
    return fig

def print_statistics(df):
    """Print flight statistics."""
    print("\n" + "="*60)
    print("FLIGHT STATISTICS")
    print("="*60)
    
    print(f"\nDuration: {df['SimulationTime'].max():.1f} seconds")
    print(f"\nAltitude:")
    print(f"  Maximum: {df['Altitude'].max():.1f} m")
    print(f"  Minimum: {df['Altitude'].min():.1f} m")
    
    print(f"\nAirspeed:")
    print(f"  Maximum: {df['Airspeed'].max():.1f} m/s")
    print(f"  Average: {df['Airspeed'].mean():.1f} m/s")
    
    print(f"\nVertical Speed:")
    print(f"  Maximum climb: {df['VerticalSpeed'].max():.1f} m/s")
    print(f"  Maximum descent: {df['VerticalSpeed'].min():.1f} m/s")
    
    print(f"\nFlight Phases:")
    for phase in df['FlightPhase'].unique():
        phase_data = df[df['FlightPhase'] == phase]
        duration = phase_data['SimulationTime'].iloc[-1] - phase_data['SimulationTime'].iloc[0]
        print(f"  {phase}: {duration:.1f} seconds")
    
    print(f"\nFaults:")
    total_faults = df['ActiveFaults'].sum()
    max_faults = df['ActiveFaults'].max()
    print(f"  Total fault-seconds: {total_faults:.0f}")
    print(f"  Maximum concurrent faults: {max_faults:.0f}")
    
    sensor_valid_pct = (df['SensorValid'] == True).sum() / len(df) * 100
    print(f"\nSensor Health:")
    print(f"  Valid readings: {sensor_valid_pct:.1f}%")
    
    print("\n" + "="*60 + "\n")

def main():
    if len(sys.argv) < 2:
        print("Usage: python visualize_telemetry.py <telemetry_csv_file>")
        print("\nExample: python visualize_telemetry.py logs/flight_data_20250125_103045.csv")
        sys.exit(1)
    
    filename = sys.argv[1]
    
    # Load data
    df = load_telemetry(filename)
    
    # Print statistics
    print_statistics(df)
    
    # Create visualizations
    print("Generating visualizations...")
    
    fig1 = plot_flight_profile(df)
    fig2 = plot_control_surfaces(df)
    fig3 = plot_sensor_data(df)
    fig4 = plot_phase_timeline(df)
    
    # Save figures
    output_dir = "plots"
    os.makedirs(output_dir, exist_ok=True)
    
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    
    fig1.savefig(f"{output_dir}/flight_profile_{timestamp}.png", dpi=300, bbox_inches='tight')
    fig2.savefig(f"{output_dir}/control_surfaces_{timestamp}.png", dpi=300, bbox_inches='tight')
    fig3.savefig(f"{output_dir}/sensor_data_{timestamp}.png", dpi=300, bbox_inches='tight')
    fig4.savefig(f"{output_dir}/phase_timeline_{timestamp}.png", dpi=300, bbox_inches='tight')
    
    print(f"\nPlots saved to {output_dir}/ directory")
    
    # Display plots
    print("\nDisplaying plots... (close windows to exit)")
    plt.show()

if __name__ == "__main__":
    main()
