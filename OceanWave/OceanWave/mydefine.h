#pragma once

#include <cmath>

// Mathematical constants
constexpr float PI_F = 3.141592654f;
constexpr float SQRT_HALF_F = 0.707106781f;

// Physical constants
constexpr float GRAVITY = 9.81f;

// Grid dimensions (must be power of 2)
constexpr unsigned int GRID_SIZE = 128;
constexpr unsigned int GRID_SIZE_PLUS_1 = GRID_SIZE + 1;
constexpr unsigned int SPECTRUM_W = GRID_SIZE + 4;
constexpr unsigned int SPECTRUM_H = GRID_SIZE + 1;

// Image dimensions (same as grid for this simulation)
constexpr unsigned int WIDTH = GRID_SIZE;
constexpr unsigned int HEIGHT = GRID_SIZE;

// Simulation parameters structure - can be modified at runtime
struct SimParams {
    float amplitude;      // Wave scale factor (A)
    float patchSize;      // Patch size in meters
    float windSpeed;      // Wind speed in m/s
    float windDir;        // Wind direction in radians
    float dirDepend;      // Directional dependency factor
    float targetTime;     // Total simulation time
    float deltaTime;      // Time step between frames
    float lambda;         // Choppy wave factor

    // Default values
    SimParams() :
        amplitude(452.0f),
        patchSize(100.0f),
        windSpeed(100.0f),
        windDir(PI_F / 3.0f),
        dirDepend(0.07f),
        targetTime(1.0f),
        deltaTime(0.5f),
        lambda(-1.0f)
    {
    }
};