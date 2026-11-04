// Unit tests for FlightController and FaultHandler. No framework needed:
// each CHECK records a failure, and the exit code is the number of failures.
#include <iostream>
#include <string>
#include "FaultHandler.h"
#include "FlightController.h"

using namespace avionics;

static int failures = 0;

#define CHECK(cond)                                                              \
    do {                                                                         \
        if (!(cond)) {                                                           \
            std::cerr << "FAIL " << __FILE__ << ":" << __LINE__ << "  " #cond "\n"; \
            ++failures;                                                          \
        }                                                                        \
    } while (0)

static void startsInPreflightWithZeroThrottle() {
    FlightController fc;
    fc.initialize();
    CHECK(fc.getCurrentPhase() == FlightPhase::PREFLIGHT);
    CHECK(fc.getControlSurfaces().throttle == 0.0);
}

// Regression test: leaving PREFLIGHT used to require throttle > 0.5, which the
// controller itself holds at 0 in PREFLIGHT, so the transition could never fire.
static void preflightExitsOnTakeoffRoll() {
    FlightController fc;
    fc.initialize();
    fc.update(1.0, 0.0, 3.0, 0.0);
    CHECK(fc.getCurrentPhase() == FlightPhase::PREFLIGHT);
    fc.update(2.0, 0.0, 6.0, 0.0);
    CHECK(fc.getCurrentPhase() == FlightPhase::TAKEOFF);
}

static void followsTheWholeFlight() {
    FlightController fc;
    fc.initialize();
    fc.update(16.0, 0.0, 6.0, 0.0);
    CHECK(fc.getCurrentPhase() == FlightPhase::TAKEOFF);
    fc.update(25.0, 150.0, 80.0, 5.0);
    CHECK(fc.getCurrentPhase() == FlightPhase::CLIMB);
    fc.update(45.0, 3100.0, 100.0, 0.5);
    CHECK(fc.getCurrentPhase() == FlightPhase::CRUISE);

    // Cruise lasts at least 60 s of simulation time, even if the aircraft sinks.
    fc.update(100.0, 3000.0, 100.0, -5.0);
    CHECK(fc.getCurrentPhase() == FlightPhase::CRUISE);
    fc.update(106.0, 3000.0, 100.0, -5.0);
    CHECK(fc.getCurrentPhase() == FlightPhase::DESCENT);

    fc.update(300.0, 400.0, 70.0, -10.0);
    CHECK(fc.getCurrentPhase() == FlightPhase::APPROACH);
    fc.update(400.0, 40.0, 30.0, -5.0);
    CHECK(fc.getCurrentPhase() == FlightPhase::LANDING);
}

static void emergencyIsLatched() {
    FlightController fc;
    fc.initialize();
    fc.update(16.0, 0.0, 6.0, 0.0);
    fc.triggerEmergency("test");
    CHECK(fc.getCurrentPhase() == FlightPhase::EMERGENCY);
    fc.update(30.0, 150.0, 80.0, 5.0);
    CHECK(fc.getCurrentPhase() == FlightPhase::EMERGENCY);
}

static void controlOutputsStayInRange() {
    FlightController fc;
    fc.initialize();
    const double inputs[][4] = {{16, 0, 6, 0}, {25, 150, 80, 5}, {45, 3100, 100, 0.5},
                                {106, 3000, 100, -5}, {300, 400, 70, -10}, {400, 40, 30, -5}};
    for (const auto& in : inputs) {
        fc.update(in[0], in[1], in[2], in[3]);
        const ControlSurfaces c = fc.getControlSurfaces();
        CHECK(c.throttle >= 0.0 && c.throttle <= 1.0);
        CHECK(c.elevator >= -1.0 && c.elevator <= 1.0);
    }
}

static void invalidSensorIsCriticalAndRecordedOnce() {
    FaultHandler fh;
    fh.initialize();
    for (int i = 0; i < 10; ++i) fh.checkSensorHealth(3000.0, 100.0, 700.0, false);
    CHECK(!fh.isSystemSafe());
    CHECK(fh.getActiveFaults().size() == 1);
}

static void faultClearsWhenSensorRecovers() {
    FaultHandler fh;
    fh.initialize();
    fh.checkSensorHealth(3000.0, 100.0, 700.0, false);
    fh.checkSensorHealth(3000.0, 100.0, 700.0, true);
    CHECK(fh.isSystemSafe());
    CHECK(fh.getActiveFaults().empty());
}

static void outOfRangeReadingIsOnlyAWarning() {
    FaultHandler fh;
    fh.initialize();
    fh.checkSensorHealth(3000.0, 400.0, 700.0, true);
    CHECK(fh.isSystemSafe());
    CHECK(fh.getFaultCount(FaultSeverity::WARNING) == 1);
}

static void negativeAltitudeAtSpeedIsFatal() {
    FaultHandler fh;
    fh.initialize();
    fh.checkSensorHealth(-10.0, 80.0, 1013.0, true);
    CHECK(!fh.isSystemSafe());
    CHECK(fh.getFaultCount(FaultSeverity::FATAL) == 1);
}

int main() {
    startsInPreflightWithZeroThrottle();
    preflightExitsOnTakeoffRoll();
    followsTheWholeFlight();
    emergencyIsLatched();
    controlOutputsStayInRange();
    invalidSensorIsCriticalAndRecordedOnce();
    faultClearsWhenSensorRecovers();
    outOfRangeReadingIsOnlyAWarning();
    negativeAltitudeAtSpeedIsFatal();

    std::cout << (failures == 0 ? "All tests passed\n" : "Some tests failed\n");
    return failures;
}
