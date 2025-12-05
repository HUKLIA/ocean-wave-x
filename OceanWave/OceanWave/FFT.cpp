#include"FFT.h"
#include<iostream>
#include <stdio.h>
#include <stdlib.h>
#include<cstdio>
#include<cstdlib>
#include"phillips.h"

int myFFT(float* imageR,
	float* imageRI,
	float* imageTR,
	float* imageTRI) {

	for (int y = 0; y < HEIGHT; y++)
	{
		for (int x = 0; x < WIDTH; x++)
		{
			imageTR[y * WIDTH + x] = imageR[y * WIDTH + x];//red
			//green
			//blue
		}
	}

	return 0;
}

unsigned int reversebits(unsigned int num)
{
	unsigned int  NO_OF_BITS = sizeof(num) * 8;
	unsigned int reverse_num = 0, i, temp;

	for (i = 0; i < NO_OF_BITS; i++)
	{
		temp = (num & (1 << i));
		if (temp)
			reverse_num |= (1 << ((NO_OF_BITS - 1) - i));
	}

	return reverse_num;
}

void GetButterflyValues(unsigned int passIndex, unsigned int x, unsigned int& outindicesX, unsigned int& outindicesY,
	float& outweightsX, float& outweightsY) {
	int sectionWidth = 2 << passIndex;
	int halfSectionWidth = sectionWidth / 2;

	int sectionStartOffset = x & ~(sectionWidth - 1);
	int halfSectionOffset = x & (halfSectionWidth - 1);
	int sectionOffset = x & (sectionWidth - 1);

	outweightsY = sinf((float)(2.0 * PI_F * sectionOffset) / (float)sectionWidth);
	outweightsX = cosf((float)(2.0 * PI_F * sectionOffset) / (float)sectionWidth);
	//outweightsY = -outweightsY;

	outindicesX = sectionStartOffset + halfSectionOffset;
	outindicesY = sectionStartOffset + halfSectionOffset + halfSectionWidth;

	//butterfly count, length
	int ButterflyCount = (int)log2(float(WIDTH));
	int LENGTH = WIDTH;


	if (passIndex == 0)
	{
		outindicesX = reversebits(outindicesX) >> (32 - ButterflyCount) & (LENGTH - 1);
		outindicesY = reversebits(outindicesY) >> (32 - ButterflyCount) & (LENGTH - 1);
	}
}

/*
for (unsigned int y = 0; y < HEIGHT; y++) {
		for (unsigned int x = 0; x < WIDTH; x++) {
			Butterfly(x, y);
		}
}
void Butterfly(unsigned int positionX, unsigned int positionY)
{
}

void Butterfly(float imageR[HEIGHT][WIDTH], float imageG[HEIGHT][WIDTH], float imageB[HEIGHT][WIDTH],
	float imageRI[HEIGHT][WIDTH], float imageGI[HEIGHT][WIDTH], float imageBI[HEIGHT][WIDTH],
	float imageTR[HEIGHT][WIDTH], float imageTG[HEIGHT][WIDTH], float imageTB[HEIGHT][WIDTH],
	float imageTRI[HEIGHT][WIDTH], float imageTGI[HEIGHT][WIDTH], float imageTBI[HEIGHT][WIDTH]);
{
	for (unsigned int y = 0; y < HEIGHT; y++) {
		for (unsigned int x = 0; x < WIDTH; x++) {
			......
			......
			......
			imageR[y * WIDTH + x]= inputR1_R + Weights.x * inputR2_R - Weights.y * inputI2_R;
			imageG[y * WIDTH + x]= inputR1_G + Weights.x * inputR2_G - Weights.y * inputI2_G;
			.......
		}
}
}

	Butterfly(imageTR, imageTG, float imageTB,
				imageTRI, imageTGI imageTBI);

*/



void ButterflyFFTX(float* imageR,
	float* imageRI,
	float* imageTR,
	float* imageTRI, int PassIndex)
{

	for (unsigned int y = 0; y < HEIGHT; y++) {
		for (unsigned int x = 0; x < WIDTH; x++) {


			int textureSampleX = x;
			unsigned int  IndicesX;
			unsigned int  IndicesY;
			float WeightsX;
			float WeightsY;


			GetButterflyValues(PassIndex, textureSampleX, IndicesX, IndicesY, WeightsX, WeightsY);
			//
			float inputR1R = imageR[y * WIDTH + IndicesX];


			//float3 inputI1 = TextureSourceI.Load(uint3(Indices.x, position.y, 0));
			float inputI1R = imageRI[y * WIDTH + IndicesX];


			//float3 inputR2 = TextureSourceR.Load(uint3(Indices.y, position.y, 0));
			float inputR2R = imageR[y * WIDTH + IndicesY];

			//float3 inputI2 = TextureSourceI.Load(uint3(Indices.y, position.y, 0));
			float inputI2R = imageRI[y * WIDTH + IndicesY];


			//TextureTargetR[position.xy] = inputR1 + Weights.x * inputR2 - Weights.y * inputI2;
			imageTR[y * WIDTH + x] = inputR1R + WeightsX * inputR2R - WeightsY * inputI2R;


			//TextureTargetI[position.xy] = inputI1 + Weights.y * inputR2 + Weights.x * inputI2;
			imageTRI[y * WIDTH + x] = inputI1R + WeightsY * inputR2R + WeightsX * inputI2R;

		}
	}
}



void ButterflyFFTY(float* imageR,
	float* imageRI,
	float* imageTR,
	float* imageTRI, int PassIndex)
{


	for (unsigned int y = 0; y < HEIGHT; y++) {
		for (unsigned int x = 0; x < WIDTH; x++) {

			int textureSampleX = y;

			unsigned int  IndicesX;
			unsigned int  IndicesY;
			float WeightsX;
			float WeightsY;

			GetButterflyValues(PassIndex, textureSampleX, IndicesX, IndicesY, WeightsX, WeightsY);

			//float3 inputR1 = TextureSourceR.Load(uint3(position.x, Indices.x, 0));
			float inputR1R = imageR[IndicesX * WIDTH + x];


			//float3 inputI1 = TextureSourceI.Load(uint3(position.x, Indices.x, 0));
			float inputI1R = imageRI[IndicesX * WIDTH + x];



			//float3 inputR2 = TextureSourceR.Load(uint3(position.x, Indices.y, 0));
			float inputR2R = imageR[IndicesY * WIDTH + x];


			//float3 inputI2 = TextureSourceI.Load(uint3(position.x, Indices.y, 0));
			float inputI2R = imageRI[IndicesY * WIDTH + x];


			//TextureTargetR[position.xy] = inputR1 + Weights.x * inputR2 - Weights.y * inputI2;
			imageTR[y * WIDTH + x] = inputR1R + WeightsX * inputR2R - WeightsY * inputI2R;

			//TextureTargetI[position.xy] = inputI1 + Weights.y * inputR2 + Weights.x * inputI2;
			imageTRI[y * WIDTH + x] = inputI1R + WeightsY * inputR2R + WeightsX * inputI2R;

		}

	}
}



void ButterflyIFFTX(float* imageR,
	float* imageRI,
	float* imageTR,
	float* imageTRI, int PassIndex)
{
	for (unsigned int y = 0; y < HEIGHT; y++) {
		for (unsigned int x = 0; x < WIDTH; x++) {

			int textureSampleX = x;

			unsigned int  IndicesX;
			unsigned int  IndicesY;
			float WeightsX;
			float WeightsY;

			GetButterflyValues(PassIndex, textureSampleX, IndicesX, IndicesY, WeightsX, WeightsY);

			//float3 inputR1 = TextureSourceR.Load(uint3(Indices.x, position.y, 0));
			float inputR1R = imageR[y * WIDTH + IndicesX];


			//float3 inputI1 = TextureSourceI.Load(uint3(Indices.x, position.y, 0));
			float inputI1R = imageRI[y * WIDTH + IndicesX];


			//float3 inputR2 = TextureSourceR.Load(uint3(Indices.y, position.y, 0));
			float inputR2R = imageR[y * WIDTH + IndicesY];


			//float3 inputI2 = TextureSourceI.Load(uint3(Indices.y, position.y, 0));
			float inputI2R = imageRI[y * WIDTH + IndicesY];


			//TextureTargetR[position.xy] = (inputR1 + Weights.x * inputR2 + Weights.y * inputI2) * 0.5;
			imageTR[y * WIDTH + x] = (inputR1R + WeightsX * inputR2R + WeightsY * inputI2R) * 0.5f;


			//TextureTargetI[position.xy] = (inputI1 - Weights.y * inputR2 + Weights.x * inputI2) * 0.5;
			imageTRI[y * WIDTH + x] = (inputI1R - WeightsY * inputR2R + WeightsX * inputI2R) * 0.5f;


		}
	}
}




void ButterflyIFFTY(float* imageR,
	float* imageRI,
	float* imageTR,
	float* imageTRI, int PassIndex)
{
	for (unsigned int y = 0; y < HEIGHT; y++) {
		for (unsigned int x = 0; x < WIDTH; x++) {
			int textureSampleX = y;

			unsigned int  IndicesX;
			unsigned int  IndicesY;
			float WeightsX;
			float WeightsY;

			GetButterflyValues(PassIndex, textureSampleX, IndicesX, IndicesY, WeightsX, WeightsY);


			//float3 inputR1 = TextureSourceR.Load(uint3(position.x, Indices.x, 0));
			float inputR1R = imageR[IndicesX * WIDTH + x];


			//float3 inputI1 = TextureSourceI.Load(uint3(position.x, Indices.x, 0));
			float inputI1R = imageRI[IndicesX * WIDTH + x];


			//float3 inputR2 = TextureSourceR.Load(uint3(position.x, Indices.y, 0));
			float inputR2R = imageR[IndicesY * WIDTH + x];


			//float3 inputI2 = TextureSourceI.Load(uint3(position.x, Indices.y, 0));
			float inputI2R = imageRI[IndicesY * WIDTH + x];


			//TextureTargetR[position.xy] = (inputR1 + Weights.x * inputR2 + Weights.y * inputI2) * 0.5;
			imageTR[y * WIDTH + x] = (inputR1R + WeightsX * inputR2R + WeightsY * inputI2R) * 0.5f;



			//TextureTargetI[position.xy] = (inputI1 - Weights.y * inputR2 + Weights.x * inputI2) * 0.5;
			imageTRI[y * WIDTH + x] = (inputI1R - WeightsY * inputR2R + WeightsX * inputI2R) * 0.5f;

		}
	}
}
