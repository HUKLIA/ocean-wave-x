#include <stdio.h>
#include <stdlib.h>
#include<cstdio>
#include<cstdlib>
#include <windows.h>
#include <complex>
#include "phillips.h"
#include"main.h"

int main(int argc, char* argv[])
{
	printf("Start to generate OBJFile\n");
	printf(" \n");

	test(argc, argv);
	return 0;

}

#define FFT_TEST 1
#define PHILLIPS_TEST 1

void IFFT_2D(const unsigned int mButterflyCount, float *imageRN, float *imageGIN, float *imageTR, float *imageTRI)
{
	for (int i = 0; i < mButterflyCount; i++)
	{
		ButterflyIFFTY(imageRN,
			imageGIN,
			imageTR,
			imageTRI, i);

		swap<float *>(imageRN, imageTR);
		swap<float *>(imageGIN, imageTRI);

	}

	for (int i = 0; i < mButterflyCount; i++)
	{
		ButterflyIFFTX(imageRN,
			imageGIN,
			imageTR,
			imageTRI, i);

		swap<float *>(imageRN, imageTR);
		swap<float *>(imageGIN, imageTRI);

	}
}

bool outputPPM(std::string str, const float *imageR, const float *imageG, const float *imageB)
{
	std::ofstream fout(str);
	if (fout.fail()) return false; //error

	constexpr int sc = 255;

	fout << "P3\n";
	fout << WIDTH << " " << HEIGHT << "\n";
	fout << "255\n";



	for (int y = 0; y < HEIGHT; y++) {

		for (int x = 0; x < WIDTH; x++) {
			fout << sc * imageR[y * WIDTH + x] << " ";//red
			fout << sc * imageG[y * WIDTH + x] << " ";//green
			fout << sc * imageB[y * WIDTH + x] << " ";//blue
		}

	}


	fout.close();
	return true; //no error
}


bool outputOBJ1(std::string str, const float *imagedispacement, const float *imagedispacementDX, const float *imagedispacementDY, const float * htsx1, const float * htsy1) {

	ofstream fout(str);
	if (fout.fail()) return false; //error

	fout << "o Mesh" << endl;

	for (int y = 0; y < HEIGHT; y++) {
		for (int x = 0; x < WIDTH; x++) {

			float iXY = 0.1;

			float ox = x + imagedispacementDX[y * WIDTH + x] * iXY;
			float oy = y + imagedispacementDY[y * WIDTH + x] * iXY;
			float z = imagedispacement[y * WIDTH + x];
			fout << "v " << ox << " " << oy << " " << z << endl; // x = WIDTH , Y = HEIGHT


		}
	}

	for (int y = 0; y < HEIGHT; y++) {
		for (int x = 0; x < WIDTH; x++) {
			//float z = imagedispacement[y * WIDTH + x];
			//vector3(0.0f - h_tilde_slopex[index].a,
			// 1.0f,
			// 0.0f - h_tilde_slopez[index].a).unit();
			float nx = (0.0f - htsx1[y * WIDTH + x]);
			float ny = (0.0f - htsy1[y * WIDTH + x]);
			float nz = 1.0f;
			float w = sqrt(nx * nx + ny * ny + nz * nz);
				nx /= w;
				ny /= w;
				nz /= w;

			fout << "vn " << nx << " " << ny << " " << nz << endl; // x = WIDTH , Y = HEIGHT
			
		}
	}

	fout << "vt " << 0 << " " << 0 << endl;

	for (int y = 0; y < HEIGHT - 1; y++) {
		for (int x = 0; x < WIDTH - 1; x++) {
			int p = y * WIDTH + x + 1;
			//		float z = sin(x)*sin(y);
			//fout << "f " << 1 << " " << 2 << " " << WIDTH + 1  << endl;
			//fout << "f " << 2 << " " << WIDTH + 1 << " " << WIDTH + 2 << endl;
			int NPoint = p + 1;
			int nexY = p + WIDTH;
			int secondp = p + WIDTH + 1;
			//fout << "f " << p << " " << NPoint << " " << secondp << " " << nexY << endl;
			fout << "f " << p << "/1/" << p ;
			fout << " " << NPoint << "/1/" << NPoint ;
			fout << " " << secondp << "/1/" << secondp ;
			fout << " " << nexY << "/1/" << nexY << endl;
		}
		//int P = 1 ;
		// int nextpoint = P + 1;
		//X P nextpoint
		// int secondP =  P+WIDTH + 1;
		//y P+WIDTH  , secondP;
	}
	fout.close();
	return true; //no error
}



int test(int argc, char* argv[]) {

	float *imageR = new float[WIDTH*HEIGHT];
	float *imageG = new float[WIDTH*HEIGHT];
	float *imageB = new float[WIDTH*HEIGHT];
	float *imageRI = new float[WIDTH*HEIGHT];
	float *imageGI = new float[WIDTH*HEIGHT];
	float *imageBI = new float[WIDTH*HEIGHT];
	float *imageTR = new float[WIDTH*HEIGHT];
	float *imageTG = new float[WIDTH*HEIGHT];
	float *imageTB = new float[WIDTH*HEIGHT];
	float *imageTRI = new float[WIDTH*HEIGHT];
	float *imageTGI = new float[WIDTH*HEIGHT];
	float *imageTBI = new float[WIDTH*HEIGHT];
	float *imageGIN = new float[WIDTH*HEIGHT];
	float *imageRN = new float[WIDTH*HEIGHT];
	float *htdx1 = new float[WIDTH*HEIGHT];
	float *htdx2 = new float[WIDTH*HEIGHT];
	float *htdy1 = new float[WIDTH*HEIGHT];
	float *htdy2 = new float[WIDTH*HEIGHT];
	float *htsx1 = new float[WIDTH*HEIGHT];
	float *htsx2 = new float[WIDTH*HEIGHT];
	float *htsy1 = new float[WIDTH*HEIGHT];
	float *htsy2 = new float[WIDTH*HEIGHT];
	float *imagedispacementDX = new float[HEIGHT*WIDTH];
	float *imagedispacementDY = new float[HEIGHT*WIDTH];
	float *SlopeX = new float[HEIGHT*WIDTH];
	float *SlopeY = new float[HEIGHT*WIDTH];
	


	std::complex<float> *ht = new std::complex<float>[spectrumW * spectrumH];

	float *imagedispacement = new float[HEIGHT*WIDTH];
	for (int y = 0; y < HEIGHT; y++) {

		for (int x = 0; x < WIDTH; x++) {
			imagedispacement[y * WIDTH + x] = sinf(x) * sinf(y); //cal
		}
	}


	const unsigned int rowPitch = WIDTH;
	const unsigned int cellPitch = rowPitch >> 3;
	const unsigned int cellHeight = WIDTH >> 3;
	const unsigned int imageSize = rowPitch * HEIGHT;
	for (unsigned int n = 0; n < imageSize; n++)
	{
		unsigned int x = n % rowPitch;
		unsigned int y = n / rowPitch;
		unsigned int i = x / cellPitch;
		unsigned int j = y / cellHeight;

		if (i % 2 == j % 2)
		{
			imageR[y * WIDTH + x] = 0;//red
			imageG[y * WIDTH + x] = 0;//green
			imageB[y * WIDTH + x] = 0;//blue
			imageRN[y * WIDTH + x] = 0;
			imageGIN[y * WIDTH + x] = 0;
		}
		else
		{
			imageR[y * WIDTH + x] = 1.0;//red
			imageG[y * WIDTH + x] = 1.0;//green
			imageB[y * WIDTH + x] = 1.0;//blue
			imageRN[y * WIDTH + x] = 1.0;//real
			imageGIN[y * WIDTH + x] = 1.0;//green image
		}
		imageRI[y * WIDTH + x] = 0;//red
		imageGI[y * WIDTH + x] = 0;//green
		imageBI[y * WIDTH + x] = 0;//blue	
		imageRN[y * WIDTH + x] = 0;
		imageGIN[y * WIDTH + x] = 0;
	}

#if PHILLIPS_TEST

	float t = 0.0f;
	float dt = 0.5f;
	float target_t = 1.0f;
	float TAA = A;

	if (argc > 1)
	{
		target_t = atof(argv[1]);
		cout << "User parameter set Target t = " << target_t << endl;
	}
	if (argc > 2)
	{
		dt = atof(argv[2]);
		cout << "User parameter set dt = " << dt << endl;
	}
	if (argc > 3)
	{
		TAA = atof(argv[3]);
		cout << "User parameter set A = " << TAA;
		cout << " \n" << endl;
	}


	unsigned int FileCount = 0;
	float totalMaxDisplacement = 0.0f;
	while (t <= target_t)
	{
		GenerateTextureSpectrumData(imageR, imageG, imageB, imageRN, imageGIN, t,
			htdx1, htdy1, htsx1, htsy1, 
			htdx2, htdy2, htsx2, htsy2, TAA);

#endif
#if FFT_TEST

		const int mButterflyCount = (int)log2(float(WIDTH));

#if 0



		for (int y = 0; y < HEIGHT; y++) {

			for (int x = 0; x < WIDTH; x++) {
				imageRN[y * WIDTH + x] = imageR[y * WIDTH + x];
				imageGIN[y * WIDTH + x] = imageRI[y * WIDTH + x];
			}
		}

		for (int i = 0; i < mButterflyCount; i++)
		{
			ButterflyFFTX(imageRN,
				imageGIN,
				imageTR,
				imageTRI, i);

			swap<float *>(imageRN, imageTR);
			swap<float *>(imageGIN, imageTRI);

		}

		for (int i = 0; i < mButterflyCount; i++)
		{
			ButterflyFFTY(imageRN,
				imageGIN,
				imageTR,
				imageTRI, i);

			swap<float *>(imageRN, imageTR);
			swap<float *>(imageGIN, imageTRI);

		}

#endif // 0
		IFFT_2D(mButterflyCount, imageRN, imageGIN, imageTR, imageTRI);
		IFFT_2D(mButterflyCount, htdx1, htdx2, imageTR, imageTRI);
		IFFT_2D(mButterflyCount, htdy1, htdy2, imageTR, imageTRI);
		IFFT_2D(mButterflyCount, htsx1, htsx2, imageTR, imageTRI);
		IFFT_2D(mButterflyCount, htsy1, htsy2, imageTR, imageTRI);
		// change hight
#if 1

		float maxDisplacement = 0.0f;
		float lambda = -1.0f;
		float sign;
		float signs[] = { 1.0f, -1.0f };
		for (int y = 0; y < HEIGHT; y++) {
			for (int x = 0; x < WIDTH; x++) {
				sign = signs[(x + y) & 1];

				//height
				const float disp = imageRN[y * WIDTH + x] * sign; //cal
				imagedispacement[y * WIDTH + x] = disp;
				maxDisplacement = max(fabs(disp), maxDisplacement);

				imagedispacementDX[y * WIDTH + x] = htdx1[y * WIDTH + x] * sign * lambda;
				imagedispacementDY[y * WIDTH + x] = htdy1[y * WIDTH + x] * sign * lambda;
		
						//float htilde = ht_RE[y * NPlus + x];

				SlopeX[y * WIDTH + x] = htsx1[y * WIDTH + x] * sign;
				SlopeY[y * WIDTH + x] = htsy1[y * WIDTH + x] * sign;
				//h_tilde_slopex[index] = h_tilde_slopex[index] * sign;
				//h_tilde_slopez[index] = h_tilde_slopez[index] * sign;

				//height
			}
		}
#endif // displacement
		//vertices[index1].y = h_tilde[index].a;
#endif 


		for (int y = 0; y < HEIGHT; y++) {
			for (int x = 0; x < WIDTH; x++) {

				imageR[y * WIDTH + x] = 0;
				imageB[y * WIDTH + x] = 0;
				imageG[y * WIDTH + x] = fabs(imagedispacement[y * WIDTH + x]);
			}
		}
		string ObjFilename = "3dtest";
		string ObjExt = ".obj";
		stringstream ss;
		ss << ObjFilename << FileCount << ObjExt;
		
		totalMaxDisplacement = max(maxDisplacement, totalMaxDisplacement);

		if (!outputOBJ1(ss.str(), imagedispacement, imagedispacementDX, imagedispacementDY, SlopeX, SlopeY)) { return -1; }

		
		cout << "Generated Ocean OBJ 3D Model Successfully at t = " << t;
		cout << " " << "Max Displacement = " << maxDisplacement;
		cout << " " << "Total Max = " << totalMaxDisplacement;
		cout << endl;


		FileCount++;
		t += dt;
	}
	cout << "Total Max = " << totalMaxDisplacement << endl;

if (!outputPPM("TRGB.ppm", imageTR, imageTG, imageTB)) { return -1; }
if (!outputPPM("RGB.ppm", imageR, imageG, imageB)) { return -1; }
if (!outputPPM("RGBI.ppm", imageRI, imageGI, imageBI)) { return -1; }
if (!outputPPM("TRGBI.ppm", imageTRI, imageTGI, imageTBI)) { return -1; }

	//if (!anotherOutputObj("test.obj")) { return -1; }
return 0;//no error
}