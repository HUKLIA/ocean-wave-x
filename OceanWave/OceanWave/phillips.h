#pragma once
#include <complex>

#include"mydefine.h"

typedef unsigned int UINT;

using namespace std;

/*

void complexAdd(float &outReal, float &outImage,
	float a, float b, float c, float d);

void complexMul(float &outReal, float &outImage,
	float a, float b, float c, float d);

*/

float phillips(float Kx, float Ky, float Vdir, float V, float dir_depend, float TA);

float urand();

float gauss();

void GenerateTextureSpectrumData(float* imageR, float* imageG, float* imageB, float* imageRN, float* imageGIN, float t,
	float* htdx1, float* htdy1, float* htsx1, float* htsy1,
	float* htdx2, float* htdy2, float* htsx2, float* htsy2, float TA);

void generate_h0(complex<float>* h0, float TA);

complex<float> htilde0(int n_prime, int m_prime, float TA);
