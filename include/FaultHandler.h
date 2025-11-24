#ifndef FAULT_HANDLER_H
#define FAULT_HANDLER_H

#include <string>
#include <vector>
#include <chrono>

namespace avionics {

/**
 * @brief Fault severity levels
 */
enum class FaultSeverity {
    INFO,
    WARNING,
    CRITICAL,
    FATAL
};

/**
 * @brief Fault record structure
 */
struct FaultRecord {
    std::chrono::system_clock::time_point timestamp;
    FaultSeverity severity;
    std::string component;
    std::string description;
    bool resolved;
};

/**
 * @brief Safety-critical fault detection and handling
 */
class FaultHandler {
public:
    FaultHandler();
    ~FaultHandler() = default;

    /**
     * @brief Initialize fault handler
     */
    void initialize();

    /**
     * @brief Report a new fault
     */
    void reportFault(FaultSeverity severity, const std::string& component, 
                    const std::string& description);

    /**
     * @brief Check sensor values for anomalies
     */
    void checkSensorHealth(double altitude, double airspeed, double pressure, bool sensorValid);

    /**
     * @brief Check control system health
     */
    void checkControlSystem(double elevator, double aileron, double rudder);

    /**
     * @brief Get all active faults
     */
    std::vector<FaultRecord> getActiveFaults() const;

    /**
     * @brief Check if system is in safe state
     */
    bool isSystemSafe() const;

    /**
     * @brief Get fault count by severity
     */
    int getFaultCount(FaultSeverity severity) const;

    /**
     * @brief Clear resolved faults
     */
    void clearResolvedFaults();

private:
    std::vector<FaultRecord> faults_;
    int criticalFaultCount_;
    int warningFaultCount_;

    /**
     * @brief Check if value is within acceptable range
     */
    bool isInRange(double value, double min, double max) const;

    /**
     * @brief Get severity string
     */
    static std::string getSeverityString(FaultSeverity severity);
};

} // namespace avionics

#endif // FAULT_HANDLER_H
