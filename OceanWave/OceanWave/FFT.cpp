#include "FFT.h"
#include <cmath>
#include <algorithm>

static unsigned int reverseBits(unsigned int num) {
    constexpr unsigned int NO_OF_BITS = sizeof(num) * 8;
    unsigned int reverse_num = 0;

    for (unsigned int i = 0; i < NO_OF_BITS; i++) {
        unsigned int temp = (num & (1 << i));
        if (temp) {
            reverse_num |= (1 << ((NO_OF_BITS - 1) - i));
        }
    }
    return reverse_num;
}

void getButterflyValues(
    unsigned int passIndex,
    unsigned int x,
    unsigned int& outIndexX,
    unsigned int& outIndexY,
    float& outWeightX,
    float& outWeightY)
{
    int sectionWidth = 2 << passIndex;
    int halfSectionWidth = sectionWidth / 2;

    int sectionStartOffset = x & ~(sectionWidth - 1);
    int halfSectionOffset = x & (halfSectionWidth - 1);
    int sectionOffset = x & (sectionWidth - 1);

    outWeightY = sinf(2.0f * PI_F * sectionOffset / static_cast<float>(sectionWidth));
    outWeightX = cosf(2.0f * PI_F * sectionOffset / static_cast<float>(sectionWidth));

    outIndexX = sectionStartOffset + halfSectionOffset;
    outIndexY = sectionStartOffset + halfSectionOffset + halfSectionWidth;

    int butterflyCount = static_cast<int>(log2(static_cast<float>(WIDTH)));

    if (passIndex == 0) {
        outIndexX = reverseBits(outIndexX) >> (32 - butterflyCount) & (WIDTH - 1);
        outIndexY = reverseBits(outIndexY) >> (32 - butterflyCount) & (WIDTH - 1);
    }
}

void butterflyFFT_X(
    float* srcReal, float* srcImag,
    float* dstReal, float* dstImag,
    int passIndex)
{
    for (unsigned int y = 0; y < HEIGHT; y++) {
        for (unsigned int x = 0; x < WIDTH; x++) {
            unsigned int indexX, indexY;
            float weightX, weightY;

            getButterflyValues(passIndex, x, indexX, indexY, weightX, weightY);

            float inputR1 = srcReal[y * WIDTH + indexX];
            float inputI1 = srcImag[y * WIDTH + indexX];
            float inputR2 = srcReal[y * WIDTH + indexY];
            float inputI2 = srcImag[y * WIDTH + indexY];

            dstReal[y * WIDTH + x] = inputR1 + weightX * inputR2 - weightY * inputI2;
            dstImag[y * WIDTH + x] = inputI1 + weightY * inputR2 + weightX * inputI2;
        }
    }
}

void butterflyFFT_Y(
    float* srcReal, float* srcImag,
    float* dstReal, float* dstImag,
    int passIndex)
{
    for (unsigned int y = 0; y < HEIGHT; y++) {
        for (unsigned int x = 0; x < WIDTH; x++) {
            unsigned int indexX, indexY;
            float weightX, weightY;

            getButterflyValues(passIndex, y, indexX, indexY, weightX, weightY);

            float inputR1 = srcReal[indexX * WIDTH + x];
            float inputI1 = srcImag[indexX * WIDTH + x];
            float inputR2 = srcReal[indexY * WIDTH + x];
            float inputI2 = srcImag[indexY * WIDTH + x];

            dstReal[y * WIDTH + x] = inputR1 + weightX * inputR2 - weightY * inputI2;
            dstImag[y * WIDTH + x] = inputI1 + weightY * inputR2 + weightX * inputI2;
        }
    }
}

void butterflyIFFT_X(
    float* srcReal, float* srcImag,
    float* dstReal, float* dstImag,
    int passIndex)
{
    for (unsigned int y = 0; y < HEIGHT; y++) {
        for (unsigned int x = 0; x < WIDTH; x++) {
            unsigned int indexX, indexY;
            float weightX, weightY;

            getButterflyValues(passIndex, x, indexX, indexY, weightX, weightY);

            float inputR1 = srcReal[y * WIDTH + indexX];
            float inputI1 = srcImag[y * WIDTH + indexX];
            float inputR2 = srcReal[y * WIDTH + indexY];
            float inputI2 = srcImag[y * WIDTH + indexY];

            dstReal[y * WIDTH + x] = (inputR1 + weightX * inputR2 + weightY * inputI2) * 0.5f;
            dstImag[y * WIDTH + x] = (inputI1 - weightY * inputR2 + weightX * inputI2) * 0.5f;
        }
    }
}

void butterflyIFFT_Y(
    float* srcReal, float* srcImag,
    float* dstReal, float* dstImag,
    int passIndex)
{
    for (unsigned int y = 0; y < HEIGHT; y++) {
        for (unsigned int x = 0; x < WIDTH; x++) {
            unsigned int indexX, indexY;
            float weightX, weightY;

            getButterflyValues(passIndex, y, indexX, indexY, weightX, weightY);

            float inputR1 = srcReal[indexX * WIDTH + x];
            float inputI1 = srcImag[indexX * WIDTH + x];
            float inputR2 = srcReal[indexY * WIDTH + x];
            float inputI2 = srcImag[indexY * WIDTH + x];

            dstReal[y * WIDTH + x] = (inputR1 + weightX * inputR2 + weightY * inputI2) * 0.5f;
            dstImag[y * WIDTH + x] = (inputI1 - weightY * inputR2 + weightX * inputI2) * 0.5f;
        }
    }
}

void IFFT_2D(
    unsigned int butterflyCount,
    float* dataReal, float* dataImag,
    float* tempReal, float* tempImag)
{
    // IFFT along Y axis
    for (unsigned int i = 0; i < butterflyCount; i++) {
        butterflyIFFT_Y(dataReal, dataImag, tempReal, tempImag, i);
        std::swap(dataReal, tempReal);
        std::swap(dataImag, tempImag);
    }

    // IFFT along X axis
    for (unsigned int i = 0; i < butterflyCount; i++) {
        butterflyIFFT_X(dataReal, dataImag, tempReal, tempImag, i);
        std::swap(dataReal, tempReal);
        std::swap(dataImag, tempImag);
    }
}