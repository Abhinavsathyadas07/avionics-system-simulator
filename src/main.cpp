#include <iostream>
#include <csignal>
#include <string>
#include "SystemMonitor.h"

using namespace avionics;

// Global system monitor for signal handling
SystemMonitor* g_systemMonitor = nullptr;

void signalHandler(int signal) {
    std::cout << "\n\nReceived signal " << signal << ", shutting down gracefully..." << std::endl;
    if (g_systemMonitor) {
        g_systemMonitor->shutdown();
    }
    exit(signal);
}

int main(int argc, char* argv[]) {
    // Set up signal handlers for graceful shutdown
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);
    
    // Parse command line arguments
    double simulationDuration = 450.0;  // Default 7.5 minutes: the scripted flight lands at about 425 s
    double updateRate = 10.0;            // Default 10 Hz
    bool injectTestFault = false;        // Pass --inject-fault as the third argument
    
    if (argc > 1) {
        try {
            simulationDuration = std::stod(argv[1]);
        } catch (const std::exception& e) {
            std::cerr << "Invalid duration argument. Using default: " 
                      << simulationDuration << " seconds" << std::endl;
        }
    }
    
    if (argc > 2) {
        try {
            updateRate = std::stod(argv[2]);
            if (updateRate < 1.0 || updateRate > 100.0) {
                std::cerr << "Update rate must be between 1 and 100 Hz. Using default: " 
                          << 10.0 << " Hz" << std::endl;
                updateRate = 10.0;
            }
        } catch (const std::exception& e) {
            std::cerr << "Invalid update rate argument. Using default: " 
                      << updateRate << " Hz" << std::endl;
        }
    }

    if (argc > 3 && std::string(argv[3]) == "--inject-fault") {
        injectTestFault = true;
    }
    
    // Create and initialize system monitor
    SystemMonitor systemMonitor;
    g_systemMonitor = &systemMonitor;
    
    if (!systemMonitor.initialize()) {
        std::cerr << "Failed to initialize avionics system" << std::endl;
        return 1;
    }
    
    // Run simulation
    try {
        systemMonitor.run(simulationDuration, updateRate, injectTestFault);
    } catch (const std::exception& e) {
        std::cerr << "\nSimulation error: " << e.what() << std::endl;
        systemMonitor.shutdown();
        return 1;
    }
    
    // Shutdown gracefully
    systemMonitor.shutdown();
    
    std::cout << "\nThank you for using the Avionics System Simulator!" << std::endl;
    std::cout << "Check the logs/ directory for detailed telemetry data.\n" << std::endl;
    
    return 0;
}
