#include "TelemetryLogger.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <sys/stat.h>

namespace avionics {

TelemetryLogger::TelemetryLogger(const std::string& logDirectory)
    : logDirectory_(logDirectory),
      startTime_(std::chrono::system_clock::now()) {
}

TelemetryLogger::~TelemetryLogger() {
    close();
}

bool TelemetryLogger::initialize() {
    std::cout << "[TelemetryLogger] Initializing logging system..." << std::endl;
    
    // Create logs directory if it doesn't exist
    #ifdef _WIN32
        _mkdir(logDirectory_.c_str());
    #else
        mkdir(logDirectory_.c_str(), 0755);
    #endif
    
    // Open telemetry data file
    std::string telemetryFilename = generateFilename("flight_data", "csv");
    telemetryFile_.open(logDirectory_ + "/" + telemetryFilename);
    
    if (!telemetryFile_.is_open()) {
        std::cerr << "[TelemetryLogger] ERROR: Could not open telemetry file" << std::endl;
        return false;
    }
    
    // Write CSV header
    telemetryFile_ << "Timestamp,SimulationTime,Altitude,Airspeed,Pressure,Temperature,"
                   << "VerticalSpeed,FlightPhase,Elevator,Aileron,Rudder,Throttle,"
                   << "ActiveFaults,SensorValid" << std::endl;
    
    // Open event log file
    std::string eventFilename = generateFilename("events", "log");
    eventFile_.open(logDirectory_ + "/" + eventFilename);
    
    if (!eventFile_.is_open()) {
        std::cerr << "[TelemetryLogger] ERROR: Could not open event file" << std::endl;
        return false;
    }
    
    logEvent("Telemetry logging system initialized");
    
    std::cout << "[TelemetryLogger] Logging to: " << telemetryFilename << std::endl;
    
    return true;
}

void TelemetryLogger::logData(double simulationTime, 
                              const SensorData& sensors,
                              FlightPhase phase,
                              const ControlSurfaces& controls,
                              int activeFaults) {
    if (!telemetryFile_.is_open()) {
        return;
    }
    
    telemetryFile_ << getTimestamp() << ","
                   << std::fixed << std::setprecision(2)
                   << simulationTime << ","
                   << sensors.altitude << ","
                   << sensors.airspeed << ","
                   << sensors.pressure << ","
                   << sensors.temperature << ","
                   << sensors.verticalSpeed << ","
                   << FlightController::getPhaseString(phase) << ","
                   << controls.elevator << ","
                   << controls.aileron << ","
                   << controls.rudder << ","
                   << controls.throttle << ","
                   << activeFaults << ","
                   << (sensors.valid ? "true" : "false")
                   << std::endl;
}

void TelemetryLogger::logEvent(const std::string& event) {
    if (!eventFile_.is_open()) {
        return;
    }
    
    eventFile_ << getTimestamp() << " - " << event << std::endl;
    eventFile_.flush();  // Ensure events are written immediately
}

void TelemetryLogger::close() {
    if (telemetryFile_.is_open()) {
        telemetryFile_.close();
        std::cout << "[TelemetryLogger] Telemetry file closed" << std::endl;
    }
    
    if (eventFile_.is_open()) {
        logEvent("Logging system shutdown");
        eventFile_.close();
        std::cout << "[TelemetryLogger] Event file closed" << std::endl;
    }
}

std::string TelemetryLogger::generateFilename(const std::string& prefix, 
                                              const std::string& extension) {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << prefix << "_"
       << std::put_time(std::localtime(&time), "%Y%m%d_%H%M%S")
       << "." << extension;
    
    return ss.str();
}

std::string TelemetryLogger::getTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S")
       << '.' << std::setfill('0') << std::setw(3) << ms.count();
    
    return ss.str();
}

} // namespace avionics
