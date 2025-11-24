#ifndef SYSTEM_MONITOR_H
#define SYSTEM_MONITOR_H

#include <chrono>
#include "FlightController.h"
#include "SensorProcessor.h"
#include "FaultHandler.h"
#include "TelemetryLogger.h"

namespace avionics {

/**
 * @brief Real-time system monitoring and coordination
 */
class SystemMonitor {
public:
    SystemMonitor();
    ~SystemMonitor() = default;

    /**
     * @brief Initialize all subsystems
     */
    bool initialize();

    /**
     * @brief Run simulation loop
     * @param duration Simulation duration in seconds
     * @param updateRate Update rate in Hz
     */
    void run(double duration, double updateRate = 10.0);

    /**
     * @brief Shutdown system gracefully
     */
    void shutdown();

private:
    FlightController flightController_;
    SensorProcessor sensorProcessor_;
    FaultHandler faultHandler_;
    TelemetryLogger logger_;
    
    std::chrono::steady_clock::time_point startTime_;
    double simulationTime_;

    /**
     * @brief Print current system status to console
     */
    void printStatus(const SensorData& sensors, FlightPhase phase, 
                    const ControlSurfaces& controls, int faultCount);
};

} // namespace avionics

#endif // SYSTEM_MONITOR_H
