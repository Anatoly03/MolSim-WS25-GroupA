# Performance Measurements (Task 4)

## System Configuration

### Local Machine (Windows/MINGW64)
- **CPU**: [Fill in your CPU model, e.g., Intel Core i7-12700H]
- **RAM**: [Fill in memory size, e.g., 16 GB]
- **Compiler**: GCC [Run `gcc --version` to get version]
- **Build Type**: Release (-O3 optimization enabled)
- **Date**: 2026-01-07

### CoolMUC4 Cluster (Linux)
- **CPU**: Intel Xeon Platinum 8xxx
- **Compiler**: GCC [Module version]
- **Build Type**: Release (-O3 optimization enabled)
- **Date**: [Fill in when submitting job]

---

## Test Configuration

**Input Configuration**:
- File: `input/full_nl_250_50.txt`
- Particles: 12,500
- Domain: 303 × 180 (Lx × Ly)
- Time: t=0 → t=1.0, dt=0.0005
- Iterations: 2,000
- Physics: Lennard-Jones (σ=1.2, ε=1.0, rcut=3.0) + gravity (g=-12.44)
- Thermostat: Every 1000 iterations
- I/O: **Disabled** (`--no-io`)

**Command**:
```bash
./build/MolSim.exe input/full_nl_250_50.txt \
    -t 1.0 -d 0.0005 -g -12.44 \
    --nthermostat 1000 --Lx 303 --Ly 180 \
    --sigma 1.2 --epsilon 1.0 --rcut 3.0 \
    --no-io
```

---

## Runtime Measurements

### Local Machine (3 runs average)

| Run | Runtime (s) | MUPS    | Notes |
|-----|-------------|---------|-------|
| 1   | [Fill in]   | [Fill in] |     |
| 2   | [Fill in]   | [Fill in] |     |
| 3   | [Fill in]   | [Fill in] |     |
| **Average** | [Calculate avg] | [Calculate avg] | |

**Formula**: MUPS = (12,500 × 2,000) / Runtime(s) = 25,000,000 / Runtime

### CoolMUC4 Cluster

| Run | Runtime (s) | MUPS    | Job ID | Notes |
|-----|-------------|---------|--------|-------|
| 1   | [Fill in]   | [Fill in] | [Fill in] |   |
| 2   | [Fill in]   | [Fill in] | [Fill in] |   |
| 3   | [Fill in]   | [Fill in] | [Fill in] |   |
| **Average** | [Calculate avg] | [Calculate avg] | | |

---

## Profiling Results

### Method Used
- [ ] GNU gprof
- [ ] perf (Linux)
- [ ] Intel VTune
- [ ] Intel Advisor

### Top Functions by Runtime

Fill in after running `scripts/profile_gprof.sh`:

| Function | % Time | Cumulative Time (s) | Self Time (s) | Calls |
|----------|--------|---------------------|---------------|-------|
| [Function name] | [Fill in] | [Fill in] | [Fill in] | [Fill in] |
| [Function name] | [Fill in] | [Fill in] | [Fill in] | [Fill in] |
| [Function name] | [Fill in] | [Fill in] | [Fill in] | [Fill in] |
| ...      | ...    | ...                 | ...           | ...   |

**Example** (expected output):
```
  %   cumulative   self              self     total
 time   seconds   seconds    calls  Ts/call  Ts/call  name
 65.23     8.45     8.45  2000000     0.00     0.00  calculateForce()
 18.92    10.90     2.45  2000000     0.00     0.00  ParticleContainer::forEachPair()
  8.11    11.95     1.05  2000000     0.00     0.00  calculateVelocity()
  4.87    12.58     0.63  2000000     0.00     0.00  calculatePosition()
```

### Bottleneck Analysis

**Main Performance Bottlenecks** (sorted by time):
1. **[Function/Module name]**: [XX]% of total time
   - Root cause: [Fill in, e.g., O(N²) brute-force all particle pairs]
   - Optimization suggestion: [Fill in, e.g., Use Linked Cell method to reduce to O(N)]

2. **[Function/Module name]**: [XX]% of total time
   - Root cause: [Fill in]
   - Optimization suggestion: [Fill in]

3. **[Other bottlenecks]**...

---

## Call Graph Analysis (Optional)

Visualize call graph using `gprof2dot`:
```bash
gprof build/MolSim.exe gmon.out | gprof2dot | dot -Tpng -o callgraph.png
```

Attach image or describe main call paths.

---

## Comparison & Conclusions

### Performance Summary
- **Local vs Cluster**: [Comparison results]
- **Speedup potential**: [Estimate based on profiling, e.g., 10-50x with Linked Cell]

### Key Findings
1. [Finding 1, e.g., calculateForce() takes 65% runtime, main bottleneck]
2. [Finding 2, e.g., Current implementation is O(N²) complexity]
3. [Finding 3, e.g., Significant performance gain with I/O disabled]

### Next Steps
- [ ] Implement Linked Cell or Neighbor List algorithm
- [ ] OpenMP parallelization for force calculation
- [ ] SIMD vectorization (AVX2/AVX512)
- [ ] Reduce memory allocation/copying

---

## Appendix: Raw Output

### Example Terminal Output (Final Results Only)
```
Simulation loop time: 45.234 s
Iterations: 2000 Particles: 12500 MUPS: 0.552
```

### gprof Report Snippet (Top Functions Only)
```
[Paste key portions of profiling_report.txt - focus on flat profile table]
```
