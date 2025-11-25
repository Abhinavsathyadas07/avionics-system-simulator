#include "FlightController.h"
#include <iostream>
#include <cmath>

namespace avionics {

FlightController::FlightController() 
    : currentPhase_(FlightPhase::PREFLIGHT),
      previousPhase_(FlightPhase::PREFLIGHT),
      phaseStartTime_(std::chrono::steady_clock::now()) {
    controls_.elevator = 0.0;
    controls_.aileron = 0.0;
    controls_.rudder = 0.0;
    controls_.throttle = 0.0;
}

void FlightController::initialize() {
    std::cout << "[FlightController] Initializing flight control system..." << std::endl;
    currentPhase_ = FlightPhase::PREFLIGHT;
    phaseStartTime_ = std::chrono::steady_clock::now();
    controls_ = {0.0, 0.0, 0.0, 0.0};
}

void FlightController::update(double altitude, double airspeed, double verticalSpeed) {
    // State machine logic for flight phase transitions
    switch (currentPhase_) {
        case FlightPhase::PREFLIGHT:
            if (airspeed > 5.0 && throttle > 0.5) {
                transitionToPhase(FlightPhase::TAKEOFF);
            }
            break;

        case FlightPhase::TAKEOFF:
            if (altitude > 100.0 && verticalSpeed > 2.0) {
                transitionToPhase(FlightPhase::CLIMB);
            }
            break;

        case FlightPhase::CLIMB:
            if (altitude > 3000.0 && std::abs(verticalSpeed) < 1.0) {
                transitionToPhase(FlightPhase::CRUISE);
            }
            break;

        case FlightPhase::CRUISE:
            // Check for descent initiation (simulated after some time)
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::steady_clock::now() - phaseStartTime_).count();
            if (elapsed > 60 && verticalSpeed < -1.0) {
                transitionToPhase(FlightPhase::DESCENT);
            }
            break;

        case FlightPhase::DESCENT:
            if (altitude < 500.0 && airspeed < 80.0) {
                transitionToPhase(FlightPhase::APPROACH);
            }
            break;

        case FlightPhase::APPROACH:
            if (altitude < 50.0) {
                transitionToPhase(FlightPhase::LANDING);
            }
            break;

        case FlightPhase::LANDING:
            if (altitude < 5.0 && airspeed < 20.0) {
                // Flight complete
            }
            break;

        case FlightPhase::EMERGENCY:
            // Emergency procedures - maintain current state
            break;
    }

    // Update control surfaces based on current phase and conditions
    updateControlSurfaces(altitude, airspeed, verticalSpeed);
}

void FlightController::transitionToPhase(FlightPhase newPhase) {
    if (newPhase != currentPhase_) {
        previousPhase_ = currentPhase_;
        currentPhase_ = newPhase;
        phaseStartTime_ = std::chrono::steady_clock::now();
        
        std::cout << "[FlightController] Phase transition: " 
                  << getPhaseString(previousPhase_) << " -> " 
                  << getPhaseString(currentPhase_) << std::endl;
    }
}

void FlightController::updateControlSurfaces(double altitude, double airspeed, double verticalSpeed) {
    // Simplified control surface logic based on flight phase
    switch (currentPhase_) {
        case FlightPhase::PREFLIGHT:
            controls_.throttle = 0.0;
            controls_.elevator = 0.0;
            break;

        case FlightPhase::TAKEOFF:
            controls_.throttle = 1.0;
            controls_.elevator = 0.15;  // Nose up for takeoff
            break;

        case FlightPhase::CLIMB:
            controls_.throttle = 0.9;
            controls_.elevator = 0.1;
            break;

        case FlightPhase::CRUISE:
            controls_.throttle = 0.7;
            controls_.elevator = 0.0;  // Level flight
            break;

        case FlightPhase::DESCENT:
            controls_.throttle = 0.4;
            controls_.elevator = -0.05;  // Slight nose down
            break;

        case FlightPhase::APPROACH:
            controls_.throttle = 0.3;
            controls_.elevator = -0.08;
            break;

        case FlightPhase::LANDING:
            controls_.throttle = 0.1;
            controls_.elevator = -0.1;
            break;

        case FlightPhase::EMERGENCY:
            controls_.throttle = 0.5;
            controls_.elevator = 0.0;
            break;
    }

    // Clamp control values to safe ranges
    controls_.elevator = std::clamp(controls_.elevator, -1.0, 1.0);
    controls_.aileron = std::clamp(controls_.aileron, -1.0, 1.0);
    controls_.rudder = std::clamp(controls_.rudder, -1.0, 1.0);
    controls_.throttle = std::clamp(controls_.throttle, 0.0, 1.0);
}

void FlightController::triggerEmergency(const std::string& reason) {
    std::cout << "[FlightController] EMERGENCY TRIGGERED: " << reason << std::endl;
    previousPhase_ = currentPhase_;
    currentPhase_ = FlightPhase::EMERGENCY;
    phaseStartTime_ = std::chrono::steady_clock::now();
}

std::string FlightController::getPhaseString(FlightPhase phase) {
    switch (phase) {
        case FlightPhase::PREFLIGHT: return "PREFLIGHT";
        case FlightPhase::TAKEOFF: return "TAKEOFF";
        case FlightPhase::CLIMB: return "CLIMB";
        case FlightPhase::CRUISE: return "CRUISE";
        case FlightPhase::DESCENT: return "DESCENT";
        case FlightPhase::APPROACH: return "APPROACH";
        case FlightPhase::LANDING: return "LANDING";
        case FlightPhase::EMERGENCY: return "EMERGENCY";
        default: return "UNKNOWN";
    }
}

} // namespace avionics
