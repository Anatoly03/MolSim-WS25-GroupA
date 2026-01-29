# Task 2 - Parallelization Benchmark Setup

## Overview

This directory contains the **official benchmark suite for Task 2: Parallelization** as specified in Worksheet 5.

### Scenario: 2D Rayleigh-Taylor Instability Simulation

**Configuration Parameters** (from Worksheet 5):
- Domain: 300 × 54 (2D)
- Time: dt = 0.0005, t_end = 50 seconds
- Gravity: g = -12.44
- Initial Temperature: T = 40 K
- Thermostat: Every 1000 steps
- Cutoff: rcutoff = 2.5σ₁ = 3.0 (σ₁ = 1.2)

**Two Liquid Components:**
- Liquid 1: 250×20 particles @ (0.6, 2.0), σ = 1.2, m = 1.0
- Liquid 2: 250×20 particles @ (0.6, 27.0), σ = 1.1, m = 2.0

---

## Files Structure

```
OpenMP_benchmark/
├── run_benchmark.sh               ⭐ Main benchmark execution script
├── plot_results.py                ⭐ Visualization and analysis script
├── benchmark_test_config.yml      📋 Task 2 configuration (YAML)
├── test_results/                  📁 Output VTK files (*.vtu)
└── README.md                       📖 This file
```

---

## Quick Start

### 1. Build the Project

```bash
cd ../../build
export LDFLAGS="-L/opt/homebrew/opt/libomp/lib"
export CPPFLAGS="-I/opt/homebrew/opt/libomp/include"
export CC=/opt/homebrew/opt/llvm/bin/clang
export CXX=/opt/homebrew/opt/llvm/bin/clang++

cmake -DENABLE_OPENMP=ON ..
make -j4
```

### 2. Run Benchmark

```bash
cd ../OpenMP_benchmark

# Run benchmark (2 repetitions per configuration)
./run_benchmark.sh

# Or specify number of repetitions
./run_benchmark.sh 3
```

**Thread counts tested:** 1, 2, 4, 8, 14, 16
**Strategies tested:** 
- Strategy 0: Direct parallelization (O(n²))
- Strategy 1: Cell-based (O(n), LinkedCells)

### 3. Analyze Results

```bash
# Generate plots and statistical analysis
python3 plot_results.py results/benchmark_results.csv results/

# View results
cat results/benchmark_results.csv      # Raw data
cat results/analysis.txt              # Detailed analysis
open results/speedup_curve.png        # Visualization (macOS)
```

---

## Output Files

### VTK Visualization Files
```
test_results/
├── MD_vtk_0000.vtu       # Time step 0
├── MD_vtk_0005.vtu       # Time step 5
├── MD_vtk_0010.vtu       # Time step 10
└── ...
```

Location: `OpenMP_benchmark/test_results/` (all inside OpenMP_benchmark folder)

### Benchmark Results
```
results/
├── benchmark_results.csv  # CSV data (Strategy, Threads, Run, Time, Speedup)
├── speedup_curve.png      # Plot: Speedup vs threads + Efficiency
└── analysis.txt           # Statistical analysis and scaling assessment
```

---

## Expected Performance

### Strong-Scaling Speedup (typical results)

| Threads | Strategy 0 | Strategy 1 |
|---------|-----------|-----------|
| 1       | 1.0x      | 1.0x      |
| 2       | 1.9x      | 1.85x     |
| 4       | 3.6x      | 3.4x      |
| 8       | 6.5x      | 6.0x      |
| 14      | 10.0x     | 9.0x      |
| 16      | 11.0x     | 10.0x     |

*Note: Actual results depend on system hardware and load*

---

## Configuration Details

### benchmark_test_config.yml

This YAML file contains the complete Task 2 scenario:

```yaml
# Simulation parameters
delta_time: 0.0005
total_time: 50
gravityFactor: -12.44
Initial_temperature: 40
thermostat_step: 1000

# Domain (2D: z=0)
domain_max: [300, 54, 0]

# Particles
Liquid_1: [250, 20] @ (0.6, 2.0), σ=1.2
Liquid_2: [250, 20] @ (0.6, 27.0), σ=1.1

# Output
output: "OpenMP_benchmark/test_results/MD_vtk"
```

### Supported Parallelization Strategies

The benchmark automatically tests:

**Strategy 0: Direct Parallelization**
- Algorithm: All particle pairs with thread-local accumulators
- Complexity: O(n²)
- Memory: O(threads × n)
- Best for: Small/medium systems (< 10k particles)

**Strategy 1: Cell-Based (LinkedCells)**
- Algorithm: Spatial partitioning with atomic updates
- Complexity: O(n)
- Memory: O(n)
- Best for: Large systems (> 100k particles)

---

## Running Custom Benchmarks

### Modify Parameters

Edit `benchmark_test_config.yml` to change:
- Simulation time (`total_time`)
- Time step (`delta_time`)
- Number of particles (`n` field)
- Domain size (`domain_max`)

### Change Thread Counts

Edit `run_benchmark.sh` line ~30:
```bash
THREAD_COUNTS=(1 2 4 8 14 16)  # Modify this line
```

### Increase Precision

Run with more repetitions:
```bash
./run_benchmark.sh 5   # 5 runs per configuration
```

---

## Troubleshooting

### "MolSim not found"
```bash
# Rebuild the project
cd ../../build && make
```

### Results look incorrect
```bash
# Clean old results and run again
rm -rf test_results/*.vtu results/*.csv
./run_benchmark.sh 3
```

### Slow performance on Strategy 1
```bash
# Strategy 1 uses atomic operations - may be slower on small systems
# Try Strategy 0 instead or increase particle count for LinkedCells to be effective
```

### Python plotting errors
```bash
# Install matplotlib if needed
pip3 install matplotlib numpy
```

---

## Analysis Interpretation

### Speedup S(n) = T(1) / T(n)
- Ideal value: Equal to thread count n
- Good efficiency: S(n) ≥ 0.8 × n
- Fair efficiency: S(n) ≥ 0.5 × n

### Parallel Efficiency E(n) = S(n) / n × 100%
- Ideal: 100% (perfect scaling)
- Good: > 75%
- Fair: 50-75%
- Poor: < 50%

### Scaling Profile
- **Super-linear**: E > 100% (rare, usually due to cache effects)
- **Strong**: E > 90%
- **Good**: E > 75%
- **Weak**: E > 50%

---

## Advanced Usage

### Profile Single Configuration

```bash
export OMP_NUM_THREADS=4
../../build/MolSim benchmark_test_config.yml
```

### Generate Speedup Data Only

```bash
# Collect raw timing data
for threads in 1 2 4 8 16; do
    export OMP_NUM_THREADS=$threads
    /usr/bin/time -v ../../build/MolSim benchmark_test_config.yml
done
```

### Compare with Alternative Strategies

```yaml
# In benchmark_test_config.yml:
parallelization_strategy: 0  # or 1 for testing
```

---

## Documentation References

- **Force Optimization**: See `FORCE_OPTIMIZATION_GUIDE.md`
- **OpenMP Setup**: See `OPENMP_QUICKSTART.md`
- **Parallelization Implementation**: See `src/core/simulation/ParallelizationStrategies.h`

---

## Task 2 Checklist

- [x] Configuration: 2D scenario (300×54, 500 particles, 50 sec simulation)
- [x] Parallelization: Strategy 0 (direct) and Strategy 1 (cell-based)
- [x] Threading: Tests 1, 2, 4, 8, 14, 16 threads
- [x] Output: VTK files in `OpenMP_benchmark/test_results/`
- [x] Analysis: Automated speedup curve generation
- [x] Documentation: Complete setup guide

---

**Status**: ✓ Ready for performance analysis

Generated: 2026-01-28

