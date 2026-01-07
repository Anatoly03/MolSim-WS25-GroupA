# Profiling Guide for MolSim (Task 4)

This document explains how to perform performance analysis on MolSim locally and on the CoolMUC4 cluster.

---

## Prerequisites

### Required Steps
1. **Use Release build** (-O3 optimization enabled by default):
   ```bash
   cmake -DCMAKE_BUILD_TYPE=Release ..
   make -j4
   ```

2. **Disable I/O** using `--no-io` or `-i` flag

3. **Use large-scale scenario** (≥10,000 particles):
   ```bash
   # Use provided 12500-particle input
   input/full_nl_250_50.txt
   ```

---

## Method 1: GNU gprof (Recommended, Cross-platform)

### Local (Windows/Linux)

**1. Build profiling version**:
```bash
# Use automated script (recommended)
bash scripts/profile_gprof.sh

# Or build manually:
rm -rf build_prof
mkdir build_prof
cd build_prof
cmake -DCMAKE_BUILD_TYPE=Release -DENABLE_GPROF=ON ..
make -j4
cd ..
```

**2. Run test** (generates `gmon.out`):
```bash
./build_prof/MolSim.exe input/full_nl_250_50.txt \
    -t 1.0 -d 0.0005 -g -12.44 \
    --nthermostat 1000 --Lx 303 --Ly 180 \
    --sigma 1.2 --epsilon 1.0 --rcut 3.0 \
    --no-io
```

**3. Generate report**:
```bash
# Text report
gprof build_prof/MolSim.exe gmon.out > profiling_report.txt

# View top 20 lines (most time-consuming functions)
head -n 50 profiling_report.txt

# Visualization (optional, requires graphviz)
gprof build_prof/MolSim.exe gmon.out | gprof2dot | dot -Tpng -o callgraph.png
```

**4. Interpret results**:
```
Flat profile:

Each sample counts as 0.01 seconds.
  %   cumulative   self              self     total           
 time   seconds   seconds    calls  ms/call  ms/call  name    
 65.23     8.45     8.45  2000000     0.00     0.00  calculateForce()
 18.92    10.90     2.45  2000000     0.00     0.00  ParticleContainer::forEachPair()
  8.11    11.95     1.05  2000000     0.00     0.00  calculateVelocity(double)
  4.87    12.58     0.63  2000000     0.00     0.00  calculatePosition(double)
  1.92    12.83     0.25        1   250.00   250.00  FileReader::readFile()
```

- **% time**: 占总运行时间百分比
- **self seconds**: 函数自身耗时（不含调用的子函数）
- **calls**: 调用次数
- **name**: 函数名

### CoolMUC4 Cluster

**1. Upload code**:
```bash
scp -r MolSim-WS25-GroupA/ <username>@login.coolmuc.de:~
```

**2. SSH login**:
```bash
ssh <username>@login.coolmuc.de
cd MolSim-WS25-GroupA
```

**3. Load modules** (adjust according to cluster config):
```bash
module load slurm_setup
module load gcc/12  # or other version
```

**4. Build profiling version**:
```bash
cmake -Bbuild_prof -DCMAKE_BUILD_TYPE=Release -DENABLE_GPROF=ON
cmake --build build_prof -- -j4
```

**5. Create SLURM batch script** (`job_profile.sh`):
```bash
#!/bin/bash
#SBATCH --job-name=molsim_prof
#SBATCH --output=profile_%j.out
#SBATCH --error=profile_%j.err
#SBATCH --partition=cm4_inter
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --time=00:30:00

cd $SLURM_SUBMIT_DIR

# Run test (generates gmon.out)
./build_prof/MolSim.exe input/full_nl_250_50.txt \
    -t 1.0 -d 0.0005 -g -12.44 \
    --nthermostat 1000 --Lx 303 --Ly 180 \
    -s 1.2 -e 1.0 -r 3.0 -i

# Generate report
gprof build_prof/MolSim.exe gmon.out > profiling_report_${SLURM_JOB_ID}.txt

echo "Profiling complete. Report: profiling_report_${SLURM_JOB_ID}.txt"
```

**6. Submit job**:
```bash
sbatch job_profile.sh
# 查看状态
squeue -u $USER
# 查看结果
cat profile_*.out
cat profiling_report_*.txt | head -n 50
```

---

## Method 2: perf (Linux only)

### Local Linux or CoolMUC4

**1. Record performance data**:
```bash
# Release构建（不需要-pg）
cmake -Bbuild -DCMAKE_BUILD_TYPE=Release
cmake --build build -- -j4

# 使用perf记录
perf record -g ./build/MolSim.exe input/full_nl_250_50.txt \
    -t 1.0 -d 0.0005 -g -12.44 -N 1000 --Lx 303 --Ly 180 \
    -s 1.2 -e 1.0 -r 3.0 -i
```

**2. Generate report**:
```bash
# Interactive view
perf report

# Text report
perf report --stdio > perf_report.txt

# Flame graph (optional, requires FlameGraph tools)
perf script | stackcollapse-perf.pl | flamegraph.pl > flamegraph.svg
```

**3. Common perf commands**:
```bash
# 统计事件
perf stat ./build/MolSim.exe [args]

# 查看热点
perf top

# 记录调用栈
perf record -g --call-graph=dwarf ./build/MolSim.exe [args]
```

---

## Method 3: Intel VTune/Advisor (CoolMUC4, requires Intel license)

### CoolMUC4 Cluster

**1. Load Intel tools**:
```bash
module load intel
module load vtune  # or advisor
```

**2. VTune command line**:
```bash
# Hotspots分析
vtune -collect hotspots -result-dir vtune_results \
    ./build/MolSim.exe input/full_nl_250_50.txt -t 1.0 ... -i

# 查看报告
vtune -report summary -result-dir vtune_results
vtune -report hotspots -result-dir vtune_results
```

**3. Advisor（向量化分析）**:
```bash
# Survey分析
advixe-cl -collect survey -project-dir advisor_results \
    -- ./build/MolSim.exe input/full_nl_250_50.txt -t 1.0 ... -i

# Roofline分析（内存带宽）
advixe-cl -collect roofline -project-dir advisor_results \
    -- ./build/MolSim.exe input/full_nl_250_50.txt -t 1.0 ... -i
```

---

## Best Practices

### 1. Disable CPU Frequency Scaling (Local Testing)

**Linux**:
```bash
# Check current policy
cat /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor

# Set to performance mode (requires root)
echo performance | sudo tee /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor
```

**Windows**: Control Panel → Power Options → High Performance

### 2. Close Background Programs
- Close browsers, IDEs, etc.
- Temporarily disable Windows Defender real-time protection (Windows)

### 3. Multiple Test Runs for Average
```bash
# 运行3次
for i in {1..3}; do
    echo "Run $i"
    ./build/MolSim.exe input/full_nl_250_50.txt ... -i
done
```

### 4. 验证编译器优化
```bash
# 确认-O3已启用
cmake -Bbuild -DCMAKE_BUILD_TYPE=Release
grep "CMAKE_CXX_FLAGS_RELEASE" build/CMakeCache.txt
# 应该看到: -O3 或类似优化标志
```

---

## FAQ

### Q1: gmon.out not generated?
**A**: Ensure:
- Used `-DENABLE_GPROF=ON`
- Program exited normally (not Ctrl+C forced termination)
- Running in writable directory

### Q2: gprof report empty or no call information?
**A**: 
- Check if `-pg` used for both compilation and linking
- Runtime too short (<1s), increase test scale
- gprof may be incomplete on MinGW, consider perf (Linux) or VTune

### Q3: Cluster job failed?
**A**:
- Check partition name (`cm4_inter` or `cm4_large`)
- Confirm time limit is sufficient (30 minutes usually enough)
- Check `.err` file: `cat *.err`

### Q4: Abnormal MUPS value?
**A**:
- Confirm particle count and iteration count are correct
- Runtime should be 5-120 seconds
- MUPS = (N × iterations) / seconds
- Typical value: 0.5-5 MUPS (depends on hardware and algorithm)

---

## Submission Checklist

Before submitting, ensure:
- [ ] Runtime and MUPS measurements (local or cluster, ≥3 runs)
- [ ] gprof/perf/VTune profiling report
- [ ] Identified top 3-5 performance bottlenecks
- [ ] Analyzed bottleneck causes (algorithm complexity, memory access, etc.)
- [ ] Proposed optimization suggestions
- [ ] All tests use Release build + --no-io
- [ ] Test scale ≥10,000 particles

Fill results into [PERFORMANCE.md](PERFORMANCE.md).
