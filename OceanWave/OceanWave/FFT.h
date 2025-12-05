#pragma once

#include "mydefine.h"

// Get butterfly operation indices and weights for a given pass
void getButterflyValues(
    unsigned int passIndex,
    unsigned int x,
    unsigned int& outIndexX,
    unsigned int& outIndexY,
    float& outWeightX,
    float& outWeightY);

// Forward FFT butterfly operations
void butterflyFFT_X(
    float* srcReal, float* srcImag,
    float* dstReal, float* dstImag,
    int passIndex);

void butterflyFFT_Y(
    float* srcReal, float* srcImag,
    float* dstReal, float* dstImag,
    int passIndex);

// Inverse FFT butterfly operations
void butterflyIFFT_X(
    float* srcReal, float* srcImag,
    float* dstReal, float* dstImag,
    int passIndex);

void butterflyIFFT_Y(
    float* srcReal, float* srcImag,
    float* dstReal, float* dstImag,
    int passIndex);

// Complete 2D IFFT
void IFFT_2D(
    unsigned int butterflyCount,
    float* dataReal, float* dataImag,
    float* tempReal, float* tempImag);