#ifndef TELEMETRY_LOGGER_H
#define TELEMETRY_LOGGER_H

#include <string>
#include <fstream>
#include <chrono>
#include "FlightController.h"
#include "SensorProcessor.h"

namespace avionics {

/**
 * @brief Logs telemetry data for post-flight analysis
 */
class TelemetryLogger {
public:
    TelemetryLogger(const std::string& logDirectory = "logs");
    ~TelemetryLogger();

    /**
     * @brief Initialize logger and create log files
     */
    bool initialize();

    /**
     * @brief Log current system state
     */
    void logData(double simulationTime, 
                const SensorData& sensors,
                FlightPhase phase,
                const ControlSurfaces& controls,
                int activeFaults);

    /**
     * @brief Log a text event
     */
    void logEvent(const std::string& event);

    /**
     * @brief Close log files
     */
    void close();

private:
    std::string logDirectory_;
    std::ofstream telemetryFile_;
    std::ofstream eventFile_;
    std::chrono::system_clock::time_point startTime_;

    /**
     * @brief Generate timestamped filename
     */
    std::string generateFilename(const std::string& prefix, const std::string& extension);

    /**
     * @brief Get current timestamp string
     */
    std::string getTimestamp();
};

} // namespace avionics

#endif // TELEMETRY_LOGGER_H
