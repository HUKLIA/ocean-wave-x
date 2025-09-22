#pragma once
#include "mydefine.h" 

int myFFT(float *imageR,
	float *imageRI,
	float *imageTR,
	float *imageTRI);

void GetButterflyValues(unsigned int passIndex, unsigned int x, unsigned int &outindicesX, unsigned int &outindicesY,
	float &outweightsX, float &outweightsY);

void ButterflyFFTX(float *imageR,
	float *imageRI,
	float *imageTR,
	float *imageTRI, int PassIndex);

void ButterflyFFTY(float *imageR,
	float *imageRI,
	float *imageTR,
	float *imageTRI, int PassIndex);


void ButterflyIFFTX(float *imageR,
	float *imageRI,
	float *imageTR,
	float *imageTRI, int PassIndex);

void ButterflyIFFTY(float *imageR,
	float *imageRI,
	float *imageTR,
	float *imageTRI, int PassIndex);