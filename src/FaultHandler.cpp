#include "FaultHandler.h"
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <sstream>

namespace avionics {

FaultHandler::FaultHandler() 
    : criticalFaultCount_(0),
      warningFaultCount_(0) {
}

void FaultHandler::initialize() {
    std::cout << "[FaultHandler] Initializing fault detection system..." << std::endl;
    faults_.clear();
    criticalFaultCount_ = 0;
    warningFaultCount_ = 0;
}

void FaultHandler::reportFault(FaultSeverity severity, const std::string& component, 
                               const std::string& description) {
    FaultRecord fault;
    fault.timestamp = std::chrono::system_clock::now();
    fault.severity = severity;
    fault.component = component;
    fault.description = description;
    fault.resolved = false;
    
    faults_.push_back(fault);
    
    // Update counters
    if (severity == FaultSeverity::CRITICAL || severity == FaultSeverity::FATAL) {
        criticalFaultCount_++;
    } else if (severity == FaultSeverity::WARNING) {
        warningFaultCount_++;
    }
    
    // Log to console
    std::cout << "[FaultHandler] " << getSeverityString(severity) << " - "
              << component << ": " << description << std::endl;
}

void FaultHandler::checkSensorHealth(double altitude, double airspeed, 
                                     double pressure, bool sensorValid) {
    // Check for invalid sensor data
    if (!sensorValid) {
        reportFault(FaultSeverity::CRITICAL, "SensorProcessor", 
                   "Sensor fault detected - invalid data");
        return;
    }
    
    // Check altitude range
    if (!isInRange(altitude, -500.0, 15000.0)) {
        reportFault(FaultSeverity::WARNING, "AltitudeSensor", 
                   "Altitude reading out of expected range");
    }
    
    // Check airspeed range
    if (!isInRange(airspeed, 0.0, 300.0)) {
        reportFault(FaultSeverity::WARNING, "AirspeedSensor", 
                   "Airspeed reading out of expected range");
    }
    
    // Check pressure range
    if (!isInRange(pressure, 100.0, 1100.0)) {
        reportFault(FaultSeverity::WARNING, "PressureSensor", 
                   "Pressure reading out of expected range");
    }
    
    // Check for dangerous conditions
    if (altitude < 0.0 && airspeed > 50.0) {
        reportFault(FaultSeverity::FATAL, "FlightSystem", 
                   "Critical: Negative altitude with high airspeed");
    }
}

void FaultHandler::checkControlSystem(double elevator, double aileron, double rudder) {
    // Check for control surface saturation
    if (std::abs(elevator) > 0.95) {
        reportFault(FaultSeverity::WARNING, "ElevatorControl", 
                   "Elevator near saturation limit");
    }
    
    if (std::abs(aileron) > 0.95) {
        reportFault(FaultSeverity::WARNING, "AileronControl", 
                   "Aileron near saturation limit");
    }
    
    if (std::abs(rudder) > 0.95) {
        reportFault(FaultSeverity::WARNING, "RudderControl", 
                   "Rudder near saturation limit");
    }
    
    // Check for control system failure (all zeros unexpectedly)
    if (elevator == 0.0 && aileron == 0.0 && rudder == 0.0) {
        // This might be normal in certain phases, so just info level
        // More sophisticated logic would check flight phase
    }
}

std::vector<FaultRecord> FaultHandler::getActiveFaults() const {
    std::vector<FaultRecord> activeFaults;
    std::copy_if(faults_.begin(), faults_.end(), std::back_inserter(activeFaults),
                 [](const FaultRecord& fault) { return !fault.resolved; });
    return activeFaults;
}

bool FaultHandler::isSystemSafe() const {
    // System is safe if there are no critical or fatal faults
    return criticalFaultCount_ == 0;
}

int FaultHandler::getFaultCount(FaultSeverity severity) const {
    return std::count_if(faults_.begin(), faults_.end(),
                        [severity](const FaultRecord& fault) {
                            return fault.severity == severity && !fault.resolved;
                        });
}

void FaultHandler::clearResolvedFaults() {
    faults_.erase(
        std::remove_if(faults_.begin(), faults_.end(),
                      [](const FaultRecord& fault) { return fault.resolved; }),
        faults_.end()
    );
}

bool FaultHandler::isInRange(double value, double min, double max) const {
    return value >= min && value <= max;
}

std::string FaultHandler::getSeverityString(FaultSeverity severity) {
    switch (severity) {
        case FaultSeverity::INFO: return "INFO";
        case FaultSeverity::WARNING: return "WARNING";
        case FaultSeverity::CRITICAL: return "CRITICAL";
        case FaultSeverity::FATAL: return "FATAL";
        default: return "UNKNOWN";
    }
}

} // namespace avionics
