# Ocean Wave Simulation

A C++ implementation of ocean wave simulation using the Phillips spectrum and FFT.

## Building

```bash
g++ -o ocean_sim main.cpp phillips.cpp FFT.cpp -std=c++17 -O2
```

## Usage

### Command-Line Mode
```bash
./ocean_sim [options]
```

### Interactive Mode
```bash
./ocean_sim -i
```
This will prompt you to enter each parameter step by step.

## Available Options

| Option | Short | Description | Default |
|--------|-------|-------------|---------|
| `--help` | `-h` | Show help message | - |
| `--interactive` | `-i` | Interactive mode | - |
| `--time` | `-t` | Total simulation time (seconds) | 1.0 |
| `--frames` | `-n` | Number of frames to generate | 3 |
| `--amplitude` | `-a` | Wave amplitude factor | 452.0 |
| `--speed` | `-s` | Wind speed (m/s) | 100.0 |
| `--winddir` | `-w` | Wind direction (degrees) | 60.0 |
| `--patch` | `-p` | Patch size (meters) | 100.0 |
| `--choppy` | `-c` | Choppy wave factor | -1.0 |
| `--dirdep` | `-r` | Directional dependency | 0.07 |

## Examples

```bash
# Default simulation (3 frames over 1 second)
./ocean_sim

# Interactive mode - enter parameters step by step
./ocean_sim -i

# 10 frames over 5 seconds with slower wind
./ocean_sim -t 5.0 -n 10 -s 50.0

# 20 frames storm conditions
./ocean_sim -t 5.0 -n 20 -s 150.0 -a 800.0

# Calm sea
./ocean_sim -t 3.0 -n 10 -s 30.0 -a 100.0
```

## Output

The simulation generates:
- `ocean_N.obj` - 3D mesh files for each frame (importable in Blender, etc.)
- `RGB.ppm` - Spectrum data (real absolute values)
- `RGBI.ppm` - Spectrum data (imaginary)
- `TRGB.ppm` - Transformed/IFFT data (real)
- `TRGBI.ppm` - Transformed/IFFT data (imaginary)
