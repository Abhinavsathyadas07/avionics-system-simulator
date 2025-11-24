#ifndef SENSOR_PROCESSOR_H
#define SENSOR_PROCESSOR_H

#include <random>
#include <deque>

namespace avionics {

/**
 * @brief Sensor data structure
 */
struct SensorData {
    double altitude;      // meters
    double airspeed;      // m/s
    double pressure;      // hPa
    double temperature;   // Celsius
    double verticalSpeed; // m/s
    bool valid;           // Data validity flag
};

/**
 * @brief Processes and filters sensor data
 */
class SensorProcessor {
public:
    SensorProcessor();
    ~SensorProcessor() = default;

    /**
     * @brief Initialize sensor processor
     */
    void initialize();

    /**
     * @brief Simulate and process sensor readings
     * @param simulationTime Current simulation time in seconds
     */
    SensorData processSensors(double simulationTime);

    /**
     * @brief Inject sensor fault for testing
     */
    void injectFault(bool altitudeFault, bool airspeedFault, bool pressureFault);

    /**
     * @brief Check if sensors are healthy
     */
    bool areSensorsHealthy() const;

private:
    std::mt19937 rng_;
    std::normal_distribution<double> noiseDist_;
    std::deque<double> altitudeHistory_;
    std::deque<double> airspeedHistory_;
    
    bool altitudeFault_;
    bool airspeedFault_;
    bool pressureFault_;

    static constexpr size_t FILTER_WINDOW_SIZE = 5;

    /**
     * @brief Apply moving average filter
     */
    double applyFilter(std::deque<double>& history, double newValue);

    /**
     * @brief Simulate raw sensor reading
     */
    double simulateAltitude(double time);
    double simulateAirspeed(double time);
    double simulatePressure(double altitude);
    double simulateTemperature(double altitude);
};

} // namespace avionics

#endif // SENSOR_PROCESSOR_H
