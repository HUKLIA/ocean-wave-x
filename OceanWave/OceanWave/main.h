#pragma once
#include<iostream>
#include<fstream>
#include <string>
#include "mydefine.h"
#include "FFT.h"

using namespace std;

bool outputPPM(std::string str, const float* imageR, const float* imageG, const float* imageB);

int test(int argc, char* argv[]);