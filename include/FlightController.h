#ifndef FLIGHT_CONTROLLER_H
#define FLIGHT_CONTROLLER_H

#include <string>
#include <chrono>

namespace avionics {

/**
 * @brief Flight phases for the avionics system
 */
enum class FlightPhase {
    PREFLIGHT,
    TAKEOFF,
    CLIMB,
    CRUISE,
    DESCENT,
    APPROACH,
    LANDING,
    EMERGENCY
};

/**
 * @brief Control surface positions
 */
struct ControlSurfaces {
    double elevator;  // Pitch control (-1.0 to 1.0)
    double aileron;   // Roll control (-1.0 to 1.0)
    double rudder;    // Yaw control (-1.0 to 1.0)
    double throttle;  // Engine power (0.0 to 1.0)
};

/**
 * @brief Flight Control System managing aircraft state machine
 */
class FlightController {
public:
    FlightController();
    ~FlightController() = default;

    /**
     * @brief Initialize the flight controller
     */
    void initialize();

    /**
     * @brief Update the flight phase based on current conditions
     * @param altitude Current altitude in meters
     * @param airspeed Current airspeed in m/s
     * @param verticalSpeed Vertical speed in m/s
     */
    void update(double altitude, double airspeed, double verticalSpeed);

    /**
     * @brief Get current flight phase
     */
    FlightPhase getCurrentPhase() const { return currentPhase_; }

    /**
     * @brief Get current control surface positions
     */
    ControlSurfaces getControlSurfaces() const { return controls_; }

    /**
     * @brief Trigger emergency mode
     */
    void triggerEmergency(const std::string& reason);

    /**
     * @brief Get phase name as string
     */
    static std::string getPhaseString(FlightPhase phase);

private:
    FlightPhase currentPhase_;
    FlightPhase previousPhase_;
    ControlSurfaces controls_;
    std::chrono::steady_clock::time_point phaseStartTime_;

    /**
     * @brief Transition to a new flight phase
     */
    void transitionToPhase(FlightPhase newPhase);

    /**
     * @brief Calculate control surface positions based on flight phase
     */
    void updateControlSurfaces(double altitude, double airspeed, double verticalSpeed);
};

} // namespace avionics

#endif // FLIGHT_CONTROLLER_H
