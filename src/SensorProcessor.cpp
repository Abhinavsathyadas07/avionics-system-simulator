#include "SensorProcessor.h"
#include <iostream>
#include <cmath>
#include <numeric>

namespace avionics {

SensorProcessor::SensorProcessor() 
    : rng_(std::random_device{}()),
      noiseDist_(0.0, 1.0),
      altitudeFault_(false),
      airspeedFault_(false),
      pressureFault_(false) {
}

void SensorProcessor::initialize() {
    std::cout << "[SensorProcessor] Initializing sensor systems..." << std::endl;
    altitudeHistory_.clear();
    airspeedHistory_.clear();
    altitudeFault_ = false;
    airspeedFault_ = false;
    pressureFault_ = false;
}

SensorData SensorProcessor::processSensors(double simulationTime) {
    SensorData data;
    
    // Simulate raw sensor readings
    double rawAltitude = simulateAltitude(simulationTime);
    double rawAirspeed = simulateAirspeed(simulationTime);
    
    // Apply filtering
    data.altitude = applyFilter(altitudeHistory_, rawAltitude);
    data.airspeed = applyFilter(airspeedHistory_, rawAirspeed);
    
    // Calculate derived values
    data.pressure = simulatePressure(data.altitude);
    data.temperature = simulateTemperature(data.altitude);
    
    // Calculate vertical speed from altitude history
    if (altitudeHistory_.size() >= 2) {
        double altDiff = altitudeHistory_.back() - altitudeHistory_[altitudeHistory_.size() - 2];
        data.verticalSpeed = altDiff * 10.0;  // Assuming 10 Hz update rate
    } else {
        data.verticalSpeed = 0.0;
    }
    
    // Check for faults
    data.valid = !altitudeFault_ && !airspeedFault_ && !pressureFault_;
    
    if (!data.valid) {
        std::cout << "[SensorProcessor] WARNING: Sensor fault detected!" << std::endl;
    }
    
    return data;
}

void SensorProcessor::injectFault(bool altitudeFault, bool airspeedFault, bool pressureFault) {
    altitudeFault_ = altitudeFault;
    airspeedFault_ = airspeedFault;
    pressureFault_ = pressureFault;
    
    std::cout << "[SensorProcessor] Fault injection: Altitude=" << altitudeFault 
              << " Airspeed=" << airspeedFault 
              << " Pressure=" << pressureFault << std::endl;
}

bool SensorProcessor::areSensorsHealthy() const {
    return !altitudeFault_ && !airspeedFault_ && !pressureFault_;
}

double SensorProcessor::applyFilter(std::deque<double>& history, double newValue) {
    history.push_back(newValue);
    
    if (history.size() > FILTER_WINDOW_SIZE) {
        history.pop_front();
    }
    
    // Moving average filter
    double sum = std::accumulate(history.begin(), history.end(), 0.0);
    return sum / history.size();
}

double SensorProcessor::simulateAltitude(double time) {
    if (altitudeFault_) {
        return -999.0;  // Invalid reading
    }
    
    // Simulate a realistic flight profile
    double altitude = 0.0;
    
    if (time < 20.0) {
        // Ground phase
        altitude = 0.0;
    } else if (time < 50.0) {
        // Takeoff and climb
        double t = time - 20.0;
        altitude = 5.0 * t * t;  // Quadratic climb
    } else if (time < 150.0) {
        // Cruise at 3000m
        altitude = 3000.0 + 50.0 * std::sin(time * 0.1);  // Small oscillations
    } else if (time < 200.0) {
        // Descent
        double t = time - 150.0;
        altitude = 3000.0 - 15.0 * t;
    } else {
        // Approach and landing
        altitude = std::max(0.0, 2250.0 - 10.0 * (time - 200.0));
    }
    
    // Add realistic sensor noise
    double noise = noiseDist_(rng_) * 2.0;
    return std::max(0.0, altitude + noise);
}

double SensorProcessor::simulateAirspeed(double time) {
    if (airspeedFault_) {
        return -999.0;  // Invalid reading
    }
    
    double airspeed = 0.0;
    
    if (time < 15.0) {
        // Ground phase
        airspeed = 0.0;
    } else if (time < 25.0) {
        // Takeoff acceleration
        double t = time - 15.0;
        airspeed = 8.0 * t;
    } else if (time < 50.0) {
        // Climb speed
        airspeed = 80.0 + (time - 25.0) * 0.8;
    } else if (time < 150.0) {
        // Cruise speed
        airspeed = 100.0 + 10.0 * std::sin(time * 0.05);
    } else if (time < 200.0) {
        // Descent
        airspeed = 95.0;
    } else {
        // Approach and landing
        double t = time - 200.0;
        airspeed = std::max(0.0, 95.0 - 3.0 * t);
    }
    
    // Add sensor noise
    double noise = noiseDist_(rng_) * 1.5;
    return std::max(0.0, airspeed + noise);
}

double SensorProcessor::simulatePressure(double altitude) {
    if (pressureFault_) {
        return -999.0;  // Invalid reading
    }
    
    // Barometric formula: P = P0 * exp(-altitude / H)
    const double P0 = 1013.25;  // Sea level pressure in hPa
    const double H = 8500.0;     // Scale height in meters
    
    double pressure = P0 * std::exp(-altitude / H);
    
    // Add small noise
    double noise = noiseDist_(rng_) * 0.5;
    return pressure + noise;
}

double SensorProcessor::simulateTemperature(double altitude) {
    // Standard atmosphere temperature model
    const double T0 = 15.0;           // Sea level temperature in Celsius
    const double lapseRate = 0.0065;  // Temperature lapse rate (°C/m)
    
    double temperature = T0 - (lapseRate * altitude);
    
    // Add small noise
    double noise = noiseDist_(rng_) * 0.3;
    return temperature + noise;
}

} // namespace avionics
