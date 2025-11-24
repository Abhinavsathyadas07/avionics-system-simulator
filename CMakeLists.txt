cmake_minimum_required(VERSION 3.15)
project(AvionicsSystemSimulator VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler flags
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
elseif(MSVC)
    add_compile_options(/W4)
endif()

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Source files
set(SOURCES
    src/main.cpp
    src/FlightController.cpp
    src/SensorProcessor.cpp
    src/FaultHandler.cpp
    src/TelemetryLogger.cpp
    src/SystemMonitor.cpp
)

# Create executable
add_executable(avionics_simulator ${SOURCES})

# Enable testing
enable_testing()

# Test executable (to be added later)
# add_subdirectory(tests)
