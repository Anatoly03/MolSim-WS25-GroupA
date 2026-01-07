# Task 4 Performance Analysis - Quick Reference

## Completed Features

### Runtime & MUPS Measurement
- `std::chrono` timing in src/MolSim.cpp
- MUPS calculation and output
- `--no-io` flag to disable I/O

### Build Configuration
- CMake Release build with -O3 optimization
- gprof profiling support via cmake/modules/profiling.cmake

### Test Scripts
- `scripts/performance_test.bat` - Windows local test (3 runs)
- `scripts/profile_gprof.sh` - Local gprof analysis
- `scripts/run_cluster.sh` - CoolMUC4 performance test
- `scripts/run_cluster_profile.sh` - CoolMUC4 profiling

---

## Quick Commands

### Local Performance Test

```bash
cmake --build build --config Release -- -j4
./build/MolSim.exe input/full_nl_250_50.txt -t 1.0 -d 0.0005 -g -12.44 -N 1000 --Lx 303 --Ly 180 -s 1.2 -e 1.0 -r 3.0 -i
```

### Local Profiling

```bash
bash scripts/profile_gprof.sh
head -n 50 profiling_report.txt
```

### Cluster Test (CoolMUC4)

```bash
scp -r MolSim-WS25-GroupA/ <user>@login.coolmuc.de:~
ssh <user>@login.coolmuc.de
cd MolSim-WS25-GroupA
cmake -Bbuild -DCMAKE_BUILD_TYPE=Release && cmake --build build -- -j4
sbatch scripts/run_cluster.sh
cat performance_*.out
```

---

## Expected Output

### Runtime Results
```
Simulation loop time: 45.234 s
Iterations: 2000 Particles: 12500 MUPS: 0.552
```

### Profiling Results
```
  %   cumulative   self              
 time   seconds   seconds    calls  name    
 65.23     8.45     8.45  2000000  calculateForce()
 18.92    10.90     2.45  2000000  ParticleContainer::forEachPair()
  8.11    11.95     1.05  2000000  calculateVelocity(double)
  4.87    12.58     0.63  2000000  calculatePosition(double)
```

Record results in `PERFORMANCE.md`.

---

## Submission Requirements

- [ ] Runtime measurement: 3+ runs, record average
- [ ] MUPS calculation: (12500 × 2000) / Runtime(s)
- [ ] Profiling report with top 5-10 functions
- [ ] Bottleneck analysis with root causes
- [ ] Optimization suggestions
- [ ] Verify: Release build, ≥10k particles, I/O disabled, -O3 enabled

---

## Expected Performance

| Implementation | MUPS Range |
|---------------|------------|
| Current O(N²) | 0.5 - 2.5  |
| Linked Cell   | 10 - 50    |
| + OpenMP (4 cores) | 40 - 150 |

## Expected Bottlenecks

1. **calculateForce()**: 60-80% (O(N²) all-pairs calculation)
2. **ParticleContainer::forEachPair**: 10-20% (iteration overhead)
3. **calculateVelocity/Position**: 5-10% (integration)

## Optimization Directions

- **Spatial decomposition**: Linked Cell or Verlet list → O(N) complexity
- **Parallelization**: OpenMP for force calculation
- **Vectorization**: SIMD (AVX2/AVX512) for LJ computation
- **Memory layout**: Structure-of-Arrays for cache efficiency

---

## Documentation

- `PERFORMANCE.md` - Fill in measurement results
- `PROFILING.md` - Detailed profiling instructions
