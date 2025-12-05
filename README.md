# Ocean Wave Simulation

A C++ implementation of realistic ocean wave simulation using the **Phillips spectrum** and **FFT (Fast Fourier Transform)**. This project generates animated 3D ocean surface meshes as OBJ files.

## Overview

This simulation implements the Tessendorf ocean wave model, widely used in computer graphics for realistic ocean rendering. The approach works by:

1. Generating an initial wave spectrum using the Phillips spectrum model
2. Evolving the spectrum over time using dispersion relations
3. Transforming frequency-domain data to spatial domain using inverse FFT
4. Computing displacement and normal maps for realistic wave shapes

## Features

- **Phillips spectrum** wave generation for realistic wind-driven waves
- **Butterfly FFT/IFFT** implementation for efficient O(n log n) computation
- **Choppy wave displacement** (horizontal displacement for sharp wave peaks)
- **Normal/slope calculation** for proper lighting in renderers
- **Animated OBJ mesh output** ready for 3D software
- **PPM image output** for debugging and visualization

## Building

### Requirements

- C++ compiler with C++11 support
- Windows SDK (uses `<windows.h>`)

### Compilation

```bash
g++ -o ocean main.cpp phillips.cpp FFT.cpp -lm
```

Or with MSVC:

```bash
cl /EHsc main.cpp phillips.cpp FFT.cpp
```

## Usage

```bash
./ocean [target_time] [dt] [amplitude]
```

### Parameters

| Parameter | Default | Description |
|-----------|---------|-------------|
| `target_time` | 1.0 | Total simulation time in seconds |
| `dt` | 0.5 | Time step between frames |
| `amplitude` | 452.0 | Wave scale factor (A) |

### Examples

```bash
# Generate frames from t=0 to t=1.0 with default settings
./ocean

# Generate frames from t=0 to t=5.0
./ocean 5.0

# 10 seconds with 0.1s intervals (100 frames)
./ocean 10.0 0.1

# Custom amplitude for larger waves
./ocean 5.0 0.25 500
```

## Output Files

### 3D Meshes

- `3dtest0.obj`, `3dtest1.obj`, ... — Animated 3D mesh frames with vertices, normals, and faces

### Debug Images

- `RGB.ppm` — Final spectrum visualization
- `TRGB.ppm` — Transformed real component
- `RGBI.ppm` — Imaginary component
- `TRGBI.ppm` — Transformed imaginary component

## Configuration

Edit `mydefine.h` to adjust simulation parameters:

```cpp
// Grid settings
#define meshSize   128        // Grid resolution (must be power of 2)
#define HEIGHT     128        // Output height
#define WIDTH      128        // Output width

// Physical parameters
#define patchSize  100        // Physical patch size in meters
#define windDir    PI_F/3.0f  // Wind direction in radians (60°)
#define windSpeed  100.0f     // Wind speed in m/s
#define A          452.0f     // Wave amplitude scale factor
#define dirDepend  0.07f      // Directional spreading (0-1)
#define g          9.81f      // Gravitational constant
```

### Parameter Effects

| Parameter | Low Value | High Value |
|-----------|-----------|------------|
| `windSpeed` | Calm, small waves | Stormy, large waves |
| `A` | Subtle ripples | Dramatic swells |
| `dirDepend` | Unidirectional waves | More omnidirectional |
| `patchSize` | Detailed small area | Large ocean expanse |

## Algorithm

### Phillips Spectrum

The Phillips spectrum models the statistical distribution of wind-driven ocean waves:

```
P(k) = A × exp(-1/(kL)²) / k⁴ × |k̂ · ŵ|²
```

Where:
- `k` — Wave vector
- `L = V²/g` — Largest possible wave from wind velocity V
- `ŵ` — Wind direction unit vector
- `A` — Amplitude scale factor

### Time Evolution

Wave heights evolve according to the deep water dispersion relation:

```
h(k,t) = h₀(k) × exp(iωt) + h₀*(-k) × exp(-iωt)
```

Where `ω = √(g|k|)` is the angular frequency, ensuring waves propagate at physically correct speeds.

### Choppy Waves (Displacement)

Horizontal displacement creates realistic sharp wave crests:

```
D(x,t) = Σ -ik/|k| × h(k,t) × exp(ik·x)
```

### FFT Implementation

Uses the Cooley-Tukey butterfly algorithm:
- Bit-reversal permutation for input reordering
- In-place butterfly operations
- Separate X and Y passes for 2D transform

## File Structure

```
├── main.cpp        # Entry point, simulation loop, file I/O
├── main.h          # Main header declarations
├── phillips.cpp    # Phillips spectrum and htilde generation
├── phillips.h      # Phillips function declarations
├── FFT.cpp         # Butterfly FFT/IFFT implementation
├── FFT.h           # FFT function declarations
└── mydefine.h      # Global constants and parameters
```

## Code Architecture

```
main()
  └── test()
        ├── GenerateTextureSpectrumData()  # Generate h̃(k,t)
        │     ├── htilde0()                # Initial spectrum h₀
        │     └── phillips()               # Phillips spectrum P(k)
        │
        ├── IFFT_2D()                      # Transform to spatial domain
        │     ├── ButterflyIFFTY()         # Vertical pass
        │     └── ButterflyIFFTX()         # Horizontal pass
        │
        └── outputOBJ1()                   # Write mesh file
```

## Sample Output

Console output during generation:

```
Start to generate OBJFile

Generated Ocean OBJ 3D Model Successfully at t = 0 Max Displacement = 12.5432 Total Max = 12.5432
Generated Ocean OBJ 3D Model Successfully at t = 0.5 Max Displacement = 11.8921 Total Max = 12.5432
Generated Ocean OBJ 3D Model Successfully at t = 1 Max Displacement = 13.1205 Total Max = 13.1205
Total Max = 13.1205
```

## Importing OBJ Files

The generated OBJ files can be imported into:

- **Blender** — File → Import → Wavefront (.obj)
- **Maya** — File → Import
- **3ds Max** — File → Import
- **Unity** — Drag into Assets folder
- **Unreal Engine** — Import to Content Browser

## References

- Tessendorf, J. (2001). *"Simulating Ocean Water"* — SIGGRAPH Course Notes
- Phillips, O.M. (1957). *"On the generation of waves by turbulent wind"* — Journal of Fluid Mechanics
- Cooley, J.W. & Tukey, J.W. (1965). *"An algorithm for the machine calculation of complex Fourier series"*

## License

[Add your license here]

---

**Note:** This is a CPU-based implementation suitable for offline rendering. For real-time applications, consider porting the FFT to GPU using CUDA or compute shaders.
