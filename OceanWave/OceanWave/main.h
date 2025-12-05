#pragma once

#include <string>
#include "mydefine.h"

// Parse command line arguments
SimParams parseArgs(int argc, char* argv[], int& numFrames);

// Print usage information
void printUsage(const char* programName);

// Print current simulation parameters
void printParams(const SimParams& params, int numFrames);

// Output PPM image file
bool outputPPM(const std::string& filename,
    const float* imageR, const float* imageG, const float* imageB);

// Output OBJ 3D model file
bool outputOBJ(const std::string& filename,
    const float* displacement,
    const float* displacementDX, const float* displacementDY,
    const float* slopeX, const float* slopeY,
    float choppy);

// Run the ocean simulation
int runSimulation(const SimParams& params, int numFrames);