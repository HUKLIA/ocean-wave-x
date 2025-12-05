#pragma once

#include <complex>
#include "mydefine.h"

// Random number generators
float urand();
float gauss();

// Phillips spectrum calculation
float phillips(float Kx, float Ky, const SimParams& params);

// Generate initial wave spectrum h0
void generate_h0(std::complex<float>* h0, const SimParams& params);

// Generate h-tilde at time t
std::complex<float> htilde0(int n_prime, int m_prime, const SimParams& params);

// Complex exponential helper
std::complex<float> complex_exp(float arg);

// Generate spectrum data for a given time step
void generateSpectrumData(
    float* heightReal, float* heightImag,
    float* dispX_real, float* dispX_imag,
    float* dispY_real, float* dispY_imag,
    float* slopeX_real, float* slopeX_imag,
    float* slopeY_real, float* slopeY_imag,
    float t, const SimParams& params);