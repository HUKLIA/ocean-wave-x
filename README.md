# Ocean Wave Simulation Generator

A C++ application that simulates ocean waves using the Phillips spectrum and Fast Fourier Transform (FFT) to generate realistic 3D wave animations. The program creates OBJ files for 3D rendering and PPM images for visualization.

## Features

- **Realistic Wave Simulation**: Implements the Phillips spectrum for physically-based ocean wave generation
- **FFT-Based Computation**: Uses 2D Fast Fourier Transform for efficient wave height field calculation
- **Multiple Output Formats**: Generates OBJ 3D models and PPM images
- **Time-Based Animation**: Creates multiple frames for wave animation over time
- **Interactive Mode**: User-friendly menu system for parameter adjustment
- **Command-Line Interface**: Supports batch processing with customizable parameters
- **Choppy Waves**: Includes horizontal displacement for realistic wave choppiness

## Requirements

- C++ compiler with C++11 support (tested with MSVC)
- Windows operating system
- MSBuild (for building the project)

## Building

### Using MSBuild (Recommended)

1. Open the solution in Visual Studio or use MSBuild directly:
   ```bash
   msbuild OceanWave/OceanWave.vcxproj /p:Configuration=Release /p:Platform=x64
   ```

2. The executable will be generated in `x64/Release/OceanWave.exe`

### Alternative Build Methods

You can also build using other C++ build systems by compiling the source files:
- `main.cpp`
- `FFT.cpp`
- `phillips.cpp`

With headers:
- `main.h`
- `FFT.h`
- `phillips.h`
- `mydefine.h`

## Usage

### Interactive Mode

Run the executable without arguments for the interactive menu:

```bash
OceanWave.exe
```

This provides a menu to:
- Run simulations with current parameters
- Change simulation parameters
- View current parameters
- Exit the program

### Command-Line Mode

Run with command-line arguments for batch processing:

```bash
OceanWave.exe [options]
```

#### Options

- `-h, --help`: Show help message
- `-t, --time FLOAT`: Total simulation time in seconds (default: 1.0)
- `-n, --frames INT`: Number of frames to generate (default: 3)
- `-a, --amplitude FLOAT`: Wave amplitude factor (default: 452.0)
- `-s, --speed FLOAT`: Wind speed in m/s (default: 100.0)
- `-w, --winddir FLOAT`: Wind direction in degrees (default: 60.0)
- `-p, --patch FLOAT`: Patch size in meters (default: 100.0)
- `-c, --choppy FLOAT`: Choppy wave factor (default: -1.0)
- `-r, --dirdep FLOAT`: Directional dependency (default: 0.07)

#### Examples

```bash
# Interactive mode
OceanWave.exe -i

# Generate 10 frames over 5 seconds with custom wind speed
OceanWave.exe -t 5.0 -n 10 -s 50.0

# High-resolution simulation with strong winds
OceanWave.exe --time 10.0 --frames 20 --speed 120.0 --amplitude 500.0
```

## Parameters Explanation

- **Amplitude**: Controls the overall height of waves
- **Patch Size**: Size of the simulated ocean patch in meters
- **Wind Speed**: Speed of wind affecting wave generation (higher = bigger waves)
- **Wind Direction**: Direction of wind in degrees
- **Directional Dependency**: Controls how directional the waves are (0.0 = omnidirectional, higher = more directional)
- **Choppy Factor**: Controls horizontal displacement (negative values create choppy waves)
- **Total Time**: Duration of the simulation
- **Number of Frames**: How many time steps to simulate

## Output

The program generates:

### OBJ Files
- `ocean_0.obj`, `ocean_1.obj`, etc.: 3D mesh files containing vertex positions, normals, and faces for each frame
- Suitable for import into 3D modeling software like Blender, Maya, or rendering engines

### PPM Images
- `RGB.ppm`: Real part of the height field
- `RGBI.ppm`: Imaginary part of the height field
- `TRGB.ppm`: Transformed real part
- `TRGBI.ppm`: Transformed imaginary part

## Technical Details

- **Grid Size**: 256x256 points (defined by GRID_SIZE in mydefine.h)
- **FFT Implementation**: Custom 2D FFT using butterfly algorithm
- **Spectrum**: Phillips spectrum with directional spreading
- **Time Integration**: Euler integration for wave propagation

## Troubleshooting

### Build Issues
- Ensure you have Visual Studio with C++ build tools installed
- Check that the Platform Toolset matches your VS version in the .vcxproj file

### Runtime Issues
- Make sure output directory is writable
- Large simulations may require significant memory (grid size dependent)

### Performance
- Simulation time scales with grid size (O(N² log N) for FFT)
- Consider reducing grid size in mydefine.h for faster testing

## License

This project is provided as-is for educational and research purposes. Please cite appropriately if used in academic work.

## References

- Tessendorf, J. (2001). Simulating Ocean Water. In *Simulating Nature: Realistic and Interactive Techniques* (pp. 221-231). SIGGRAPH 2001 Course Notes.
- Phillips, O. M. (1957). The equilibrium range in the spectrum of wind-generated waves. *Journal of Fluid Mechanics*, 4(4), 426-434.</content>
<parameter name="filePath">

