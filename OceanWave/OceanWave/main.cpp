#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <complex>

#include "main.h"
#include "phillips.h"
#include "FFT.h"

void printUsage(const char* programName) {
    printf("Ocean Wave Simulation - Generates OBJ 3D models based on Phillips spectrum\n\n");
    printf("Usage: %s [options]\n\n", programName);
    printf("Modes:\n");
    printf("  -i, --interactive   Interactive mode (enter parameters step by step)\n");
    printf("  (no args)           Run with default parameters\n\n");
    printf("Options:\n");
    printf("  -h, --help          Show this help message\n");
    printf("  -t, --time FLOAT    Total simulation time (default: 1.0)\n");
    printf("  -n, --frames INT    Number of frames to generate (default: 3)\n");
    printf("  -a, --amplitude FLOAT   Wave amplitude factor (default: 452.0)\n");
    printf("  -s, --speed FLOAT   Wind speed in m/s (default: 100.0)\n");
    printf("  -w, --winddir FLOAT Wind direction in degrees (default: 60.0)\n");
    printf("  -p, --patch FLOAT   Patch size in meters (default: 100.0)\n");
    printf("  -c, --choppy FLOAT  Choppy wave factor (default: -1.0)\n");
    printf("  -r, --dirdep FLOAT  Directional dependency (default: 0.07)\n");
    printf("\nExamples:\n");
    printf("  %s -i                          # Interactive mode\n", programName);
    printf("  %s -t 5.0 -n 10 -s 50.0        # 10 frames over 5 seconds\n", programName);
    printf("  %s --time 10.0 --frames 20     # 20 frames over 10 seconds\n", programName);
}

void printParams(const SimParams& params, int numFrames) {
    printf("Simulation Parameters:\n");
    printf("  Amplitude:    %.2f\n", params.amplitude);
    printf("  Patch Size:   %.2f m\n", params.patchSize);
    printf("  Wind Speed:   %.2f m/s\n", params.windSpeed);
    printf("  Wind Dir:     %.2f degrees (%.4f rad)\n", params.windDir * 180.0f / PI_F, params.windDir);
    printf("  Dir Depend:   %.4f\n", params.dirDepend);
    printf("  Total Time:   %.2f s\n", params.targetTime);
    printf("  Num Frames:   %d\n", numFrames);
    printf("  Delta Time:   %.4f s\n", params.deltaTime);
    printf("  Choppy:       %.2f\n", params.lambda);
    printf("  Grid Size:    %u x %u\n\n", GRID_SIZE, GRID_SIZE);
}

SimParams parseArgs(int argc, char* argv[], int& numFrames) {
    SimParams params;
    numFrames = 3;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            printUsage(argv[0]);
            exit(0);
        }
        else if (strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--interactive") == 0) {
            // Skip, handled in main
        }
        else if ((strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--time") == 0) && i + 1 < argc) {
            params.targetTime = static_cast<float>(atof(argv[++i]));
        }
        else if ((strcmp(argv[i], "-n") == 0 || strcmp(argv[i], "--frames") == 0) && i + 1 < argc) {
            numFrames = atoi(argv[++i]);
        }
        else if ((strcmp(argv[i], "-a") == 0 || strcmp(argv[i], "--amplitude") == 0) && i + 1 < argc) {
            params.amplitude = static_cast<float>(atof(argv[++i]));
        }
        else if ((strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--speed") == 0) && i + 1 < argc) {
            params.windSpeed = static_cast<float>(atof(argv[++i]));
        }
        else if ((strcmp(argv[i], "-w") == 0 || strcmp(argv[i], "--winddir") == 0) && i + 1 < argc) {
            params.windDir = static_cast<float>(atof(argv[++i])) * PI_F / 180.0f;
        }
        else if ((strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--patch") == 0) && i + 1 < argc) {
            params.patchSize = static_cast<float>(atof(argv[++i]));
        }
        else if ((strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--choppy") == 0) && i + 1 < argc) {
            params.lambda = static_cast<float>(atof(argv[++i]));
        }
        else if ((strcmp(argv[i], "-r") == 0 || strcmp(argv[i], "--dirdep") == 0) && i + 1 < argc) {
            params.dirDepend = static_cast<float>(atof(argv[++i]));
        }
        else {
            printf("Unknown option: %s\n", argv[i]);
            printUsage(argv[0]);
            exit(1);
        }
    }

    if (numFrames > 1) {
        params.deltaTime = params.targetTime / (numFrames - 1);
    }
    else {
        params.deltaTime = params.targetTime + 1.0f;
    }

    return params;
}

bool outputPPM(const std::string& filename,
    const float* imageR, const float* imageG, const float* imageB)
{
    std::ofstream fout(filename);
    if (fout.fail()) return false;

    fout << "P3\n" << WIDTH << " " << HEIGHT << "\n255\n";

    for (unsigned int y = 0; y < HEIGHT; y++) {
        for (unsigned int x = 0; x < WIDTH; x++) {
            int idx = y * WIDTH + x;
            fout << static_cast<int>(255 * std::clamp(imageR[idx], 0.0f, 1.0f)) << " ";
            fout << static_cast<int>(255 * std::clamp(imageG[idx], 0.0f, 1.0f)) << " ";
            fout << static_cast<int>(255 * std::clamp(imageB[idx], 0.0f, 1.0f)) << " ";
        }
        fout << "\n";
    }

    fout.close();
    return true;
}

bool outputOBJ(const std::string& filename,
    const float* displacement,
    const float* displacementDX, const float* displacementDY,
    const float* slopeX, const float* slopeY,
    float choppy)
{
    std::ofstream fout(filename);
    if (fout.fail()) return false;

    fout << "# Ocean wave mesh\no OceanMesh\n\n";

    float displacementScale = 0.1f;
    for (unsigned int y = 0; y < HEIGHT; y++) {
        for (unsigned int x = 0; x < WIDTH; x++) {
            int idx = y * WIDTH + x;
            float ox = x + displacementDX[idx] * displacementScale * choppy;
            float oy = y + displacementDY[idx] * displacementScale * choppy;
            float oz = displacement[idx];
            fout << "v " << ox << " " << oy << " " << oz << "\n";
        }
    }

    fout << "\n";

    for (unsigned int y = 0; y < HEIGHT; y++) {
        for (unsigned int x = 0; x < WIDTH; x++) {
            int idx = y * WIDTH + x;
            float nx = -slopeX[idx];
            float ny = -slopeY[idx];
            float nz = 1.0f;
            float len = sqrtf(nx * nx + ny * ny + nz * nz);
            fout << "vn " << nx / len << " " << ny / len << " " << nz / len << "\n";
        }
    }

    fout << "\nvt 0 0\n\n";

    for (unsigned int y = 0; y < HEIGHT - 1; y++) {
        for (unsigned int x = 0; x < WIDTH - 1; x++) {
            int p = y * WIDTH + x + 1;
            fout << "f " << p << "/1/" << p << " ";
            fout << p + 1 << "/1/" << p + 1 << " ";
            fout << p + WIDTH + 1 << "/1/" << p + WIDTH + 1 << " ";
            fout << p + WIDTH << "/1/" << p + WIDTH << "\n";
        }
    }

    fout.close();
    return true;
}

int runSimulation(const SimParams& params, int numFrames) {
    const size_t arraySize = WIDTH * HEIGHT;
    const unsigned int butterflyCount = static_cast<unsigned int>(log2(static_cast<float>(WIDTH)));

    float* heightReal = new float[arraySize]();
    float* heightImag = new float[arraySize]();
    float* tempReal = new float[arraySize]();
    float* tempImag = new float[arraySize]();
    float* dispX_real = new float[arraySize]();
    float* dispX_imag = new float[arraySize]();
    float* dispY_real = new float[arraySize]();
    float* dispY_imag = new float[arraySize]();
    float* slopeX_real = new float[arraySize]();
    float* slopeX_imag = new float[arraySize]();
    float* slopeY_real = new float[arraySize]();
    float* slopeY_imag = new float[arraySize]();
    float* displacement = new float[arraySize]();
    float* displacementDX = new float[arraySize]();
    float* displacementDY = new float[arraySize]();
    float* finalSlopeX = new float[arraySize]();
    float* finalSlopeY = new float[arraySize]();

    float* imageR = new float[arraySize]();
    float* imageG = new float[arraySize]();
    float* imageB = new float[arraySize]();
    float* imageRI = new float[arraySize]();
    float* imageGI = new float[arraySize]();
    float* imageBI = new float[arraySize]();
    float* imageTR = new float[arraySize]();
    float* imageTG = new float[arraySize]();
    float* imageTB = new float[arraySize]();
    float* imageTRI = new float[arraySize]();
    float* imageTGI = new float[arraySize]();
    float* imageTBI = new float[arraySize]();

    float t = 0.0f;
    int frameCount = 0;
    float totalMaxDisplacement = 0.0f;

    printf("Starting simulation...\n\n");

    while (frameCount < numFrames) {
        generateSpectrumData(heightReal, heightImag, dispX_real, dispX_imag,
            dispY_real, dispY_imag, slopeX_real, slopeX_imag,
            slopeY_real, slopeY_imag, t, params);

        for (size_t i = 0; i < arraySize; i++) {
            imageR[i] = fabsf(heightReal[i]);
            imageG[i] = heightImag[i];
            imageB[i] = 0.0f;
            imageRI[i] = imageGI[i] = imageBI[i] = 0.0f;
        }

        IFFT_2D(butterflyCount, heightReal, heightImag, tempReal, tempImag);
        IFFT_2D(butterflyCount, dispX_real, dispX_imag, tempReal, tempImag);
        IFFT_2D(butterflyCount, dispY_real, dispY_imag, tempReal, tempImag);
        IFFT_2D(butterflyCount, slopeX_real, slopeX_imag, tempReal, tempImag);
        IFFT_2D(butterflyCount, slopeY_real, slopeY_imag, tempReal, tempImag);

        float maxDisplacement = 0.0f;
        const float signs[] = { 1.0f, -1.0f };

        for (unsigned int y = 0; y < HEIGHT; y++) {
            for (unsigned int x = 0; x < WIDTH; x++) {
                int idx = y * WIDTH + x;
                float sign = signs[(x + y) & 1];

                float disp = heightReal[idx] * sign;
                displacement[idx] = disp;
                maxDisplacement = std::max(fabsf(disp), maxDisplacement);

                displacementDX[idx] = dispX_real[idx] * sign * params.lambda;
                displacementDY[idx] = dispY_real[idx] * sign * params.lambda;
                finalSlopeX[idx] = slopeX_real[idx] * sign;
                finalSlopeY[idx] = slopeY_real[idx] * sign;
            }
        }

        for (size_t i = 0; i < arraySize; i++) {
            imageTR[i] = heightReal[i];
            imageTG[i] = imageTB[i] = 0.0f;
            imageTRI[i] = heightImag[i];
            imageTGI[i] = imageTBI[i] = 0.0f;
        }

        totalMaxDisplacement = std::max(maxDisplacement, totalMaxDisplacement);

        std::stringstream ss;
        ss << "ocean_" << frameCount << ".obj";

        if (!outputOBJ(ss.str(), displacement, displacementDX, displacementDY,
            finalSlopeX, finalSlopeY, params.lambda)) {
            printf("Error: Failed to write %s\n", ss.str().c_str());
            return -1;
        }

        printf("Frame %d: t=%.3f, max_disp=%.4f, total_max=%.4f -> %s\n",
            frameCount, t, maxDisplacement, totalMaxDisplacement, ss.str().c_str());

        frameCount++;
        t += params.deltaTime;
    }

    printf("\nSimulation complete!\n");
    printf("Generated %d frames\n", frameCount);
    printf("Total max displacement: %.4f\n", totalMaxDisplacement);

    printf("\nGenerating PPM files...\n");
    outputPPM("RGB.ppm", imageR, imageG, imageB);
    printf("  RGB.ppm\n");
    outputPPM("RGBI.ppm", imageRI, imageGI, imageBI);
    printf("  RGBI.ppm\n");
    outputPPM("TRGB.ppm", imageTR, imageTG, imageTB);
    printf("  TRGB.ppm\n");
    outputPPM("TRGBI.ppm", imageTRI, imageTGI, imageTBI);
    printf("  TRGBI.ppm\n");

    delete[] heightReal; delete[] heightImag; delete[] tempReal; delete[] tempImag;
    delete[] dispX_real; delete[] dispX_imag; delete[] dispY_real; delete[] dispY_imag;
    delete[] slopeX_real; delete[] slopeX_imag; delete[] slopeY_real; delete[] slopeY_imag;
    delete[] displacement; delete[] displacementDX; delete[] displacementDY;
    delete[] finalSlopeX; delete[] finalSlopeY;
    delete[] imageR; delete[] imageG; delete[] imageB;
    delete[] imageRI; delete[] imageGI; delete[] imageBI;
    delete[] imageTR; delete[] imageTG; delete[] imageTB;
    delete[] imageTRI; delete[] imageTGI; delete[] imageTBI;

    return 0;
}

int main(int argc, char* argv[]) {
    printf("========================================\n");
    printf("   Ocean Wave Simulation Generator\n");
    printf("========================================\n\n");

    SimParams params;
    int numFrames = 3;

    bool interactiveMode = false;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--interactive") == 0) {
            interactiveMode = true;
            break;
        }
    }

    if (interactiveMode) {
        printf("Interactive Mode - Enter simulation parameters:\n");
        printf("(Press Enter to use default value)\n\n");

        char input[256];

        printf("Total time (seconds) [1.0]: ");
        if (fgets(input, sizeof(input), stdin) && input[0] != '\n')
            params.targetTime = static_cast<float>(atof(input));

        printf("Number of frames [3]: ");
        if (fgets(input, sizeof(input), stdin) && input[0] != '\n')
            numFrames = atoi(input);

        printf("Wind speed (m/s) [100.0]: ");
        if (fgets(input, sizeof(input), stdin) && input[0] != '\n')
            params.windSpeed = static_cast<float>(atof(input));

        printf("Amplitude [452.0]: ");
        if (fgets(input, sizeof(input), stdin) && input[0] != '\n')
            params.amplitude = static_cast<float>(atof(input));

        printf("Wind direction (degrees) [60.0]: ");
        if (fgets(input, sizeof(input), stdin) && input[0] != '\n')
            params.windDir = static_cast<float>(atof(input)) * PI_F / 180.0f;

        printf("Patch size (meters) [100.0]: ");
        if (fgets(input, sizeof(input), stdin) && input[0] != '\n')
            params.patchSize = static_cast<float>(atof(input));

        printf("Choppy factor [-1.0]: ");
        if (fgets(input, sizeof(input), stdin) && input[0] != '\n')
            params.lambda = static_cast<float>(atof(input));

        if (numFrames > 1)
            params.deltaTime = params.targetTime / (numFrames - 1);
        else
            params.deltaTime = params.targetTime + 1.0f;

        printf("\n");
    }
    else if (argc > 1) {
        params = parseArgs(argc, argv, numFrames);
    }
    else {
        if (numFrames > 1)
            params.deltaTime = params.targetTime / (numFrames - 1);
    }

    printParams(params, numFrames);

    return runSimulation(params, numFrames);
}