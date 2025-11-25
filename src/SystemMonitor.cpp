#include "SystemMonitor.h"
#include <iostream>
#include <iomanip>
#include <thread>
#include <cmath>

namespace avionics {

SystemMonitor::SystemMonitor() 
    : simulationTime_(0.0) {
}

bool SystemMonitor::initialize() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "  AVIONICS SYSTEM SIMULATOR v1.0" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    // Initialize all subsystems
    flightController_.initialize();
    sensorProcessor_.initialize();
    faultHandler_.initialize();
    
    if (!logger_.initialize()) {
        std::cerr << "[SystemMonitor] ERROR: Failed to initialize logger" << std::endl;
        return false;
    }
    
    startTime_ = std::chrono::steady_clock::now();
    simulationTime_ = 0.0;
    
    logger_.logEvent("System initialization complete");
    
    std::cout << "[SystemMonitor] All systems initialized successfully\n" << std::endl;
    return true;
}

void SystemMonitor::run(double duration, double updateRate) {
    std::cout << "[SystemMonitor] Starting simulation..." << std::endl;
    std::cout << "Duration: " << duration << " seconds" << std::endl;
    std::cout << "Update rate: " << updateRate << " Hz\n" << std::endl;
    
    logger_.logEvent("Flight simulation started");
    
    double dt = 1.0 / updateRate;
    int iterationCount = 0;
    
    while (simulationTime_ < duration) {
        // Process sensors
        SensorData sensors = sensorProcessor_.processSensors(simulationTime_);
        
        // Update flight controller
        flightController_.update(sensors.altitude, sensors.airspeed, sensors.verticalSpeed);
        
        FlightPhase currentPhase = flightController_.getCurrentPhase();
        ControlSurfaces controls = flightController_.getControlSurfaces();
        
        // Check for faults
        faultHandler_.checkSensorHealth(sensors.altitude, sensors.airspeed, 
                                       sensors.pressure, sensors.valid);
        faultHandler_.checkControlSystem(controls.elevator, controls.aileron, controls.rudder);
        
        // Trigger emergency if critical faults detected
        if (!faultHandler_.isSystemSafe() && currentPhase != FlightPhase::EMERGENCY) {
            flightController_.triggerEmergency("Critical fault detected");
            logger_.logEvent("Emergency mode activated due to critical fault");
        }
        
        int activeFaults = faultHandler_.getActiveFaults().size();
        
        // Log telemetry data
        logger_.logData(simulationTime_, sensors, currentPhase, controls, activeFaults);
        
        // Print status every second
        if (iterationCount % static_cast<int>(updateRate) == 0) {
            printStatus(sensors, currentPhase, controls, activeFaults);
        }
        
        // Simulate fault injection at specific times (for testing)
        if (std::abs(simulationTime_ - 100.0) < dt) {
            std::cout << "\n[SystemMonitor] Injecting sensor fault for testing..." << std::endl;
            sensorProcessor_.injectFault(false, true, false);  // Airspeed fault
            logger_.logEvent("Test fault injected: Airspeed sensor");
        }
        
        if (std::abs(simulationTime_ - 105.0) < dt) {
            std::cout << "[SystemMonitor] Clearing sensor fault...\n" << std::endl;
            sensorProcessor_.injectFault(false, false, false);
            logger_.logEvent("Test fault cleared");
        }
        
        // Advance simulation time
        simulationTime_ += dt;
        iterationCount++;
        
        // Sleep to maintain real-time pace (optional)
        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(dt * 1000)));
    }
    
    logger_.logEvent("Flight simulation completed");
    std::cout << "\n[SystemMonitor] Simulation complete\n" << std::endl;
}

void SystemMonitor::shutdown() {
    std::cout << "[SystemMonitor] Shutting down systems..." << std::endl;
    
    logger_.logEvent("System shutdown initiated");
    logger_.close();
    
    // Print final statistics
    std::cout << "\n========================================" << std::endl;
    std::cout << "  SIMULATION STATISTICS" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Total simulation time: " << simulationTime_ << " seconds" << std::endl;
    std::cout << "Final phase: " 
              << FlightController::getPhaseString(flightController_.getCurrentPhase()) 
              << std::endl;
    std::cout << "Active faults: " << faultHandler_.getActiveFaults().size() << std::endl;
    std::cout << "System safe: " << (faultHandler_.isSystemSafe() ? "YES" : "NO") << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    std::cout << "[SystemMonitor] Shutdown complete" << std::endl;
}

void SystemMonitor::printStatus(const SensorData& sensors, FlightPhase phase, 
                               const ControlSurfaces& controls, int faultCount) {
    std::cout << std::fixed << std::setprecision(1);
    std::cout << "[T+" << std::setw(6) << simulationTime_ << "s] "
              << "Phase: " << std::setw(10) << std::left 
              << FlightController::getPhaseString(phase) << " | "
              << "Alt: " << std::setw(7) << std::right << sensors.altitude << "m | "
              << "Speed: " << std::setw(6) << sensors.airspeed << "m/s | "
              << "VS: " << std::setw(6) << sensors.verticalSpeed << "m/s | "
              << "Throttle: " << std::setw(4) << (controls.throttle * 100.0) << "% | "
              << "Faults: " << faultCount;
    
    if (!sensors.valid) {
        std::cout << " [SENSOR FAULT]";
    }
    
    std::cout << std::endl;
}

} // namespace avionics
