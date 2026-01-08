# Manual Build and Run Instructions

If the automatic build scripts don't work, follow these manual steps:

## Manual Build

### Linux/Mac

```bash
# 1. Create and enter build directory
mkdir -p build
cd build

# 2. Configure with CMake
cmake ..

# 3. Build
make -j4

# 4. Return to project root
cd ..

# 5. Create output directory
mkdir -p output
```

### Windows (Visual Studio)

```cmd
REM 1. Create and enter build directory
mkdir build
cd build

REM 2. Configure with CMake
cmake ..

REM 3. Build (choose one)
REM Option A: Using CMake
cmake --build . --config Release

REM Option B: Using MSBuild
msbuild MolSim.sln /p:Configuration=Release

REM 4. Return to project root
cd ..

REM 5. Create output directory
mkdir output
```

### Windows (MinGW)

```bash
# 1. Create and enter build directory
mkdir -p build
cd build

# 2. Configure with CMake for MinGW
cmake -G "MinGW Makefiles" ..

# 3. Build
mingw32-make

# 4. Return to project root
cd ..

# 5. Create output directory
mkdir -p output
```

## Manual Run

### Phase 1: Equilibration

**Linux/Mac:**
```bash
./build/MolSim input/drop-equilibration.yml
```

**Windows (Visual Studio):**
```cmd
build\Release\MolSim.exe input\drop-equilibration.yml
```

**Windows (MinGW):**
```bash
./build/MolSim.exe input/drop-equilibration.yml
```

**Expected output:**
```
[info] Loaded 12500 particles from YAML file
[info] Simulation starting...
[info] Iteration 1000 finished.
[info] Iteration 2000 finished.
...
[info] Iteration 30000 finished.
[info] Output written. Terminating...
[info] Checkpoint saved to: output/equilibrated.chkpt
```

**Expected files:**
- `output/MD_0000.vtk` (or .xyz)
- `output/MD_1000.vtk`
- `output/MD_2000.vtk`
- ... (every 1000 iterations)
- `output/equilibrated.chkpt` ← **Important!**

### Phase 2: Main Simulation

**Linux/Mac:**
```bash
./build/MolSim input/drop-simulation.yml
```

**Windows (Visual Studio):**
```cmd
build\Release\MolSim.exe input\drop-simulation.yml
```

**Windows (MinGW):**
```bash
./build/MolSim.exe input/drop-simulation.yml
```

**Expected output:**
```
[info] Loading particles from checkpoint file: output/equilibrated.chkpt
[info] Loaded 12500 particles from checkpoint file
[info] Loaded disc with ~1256 particles
[info] Simulation starting...
[info] Iteration 200 finished.
[info] Iteration 400 finished.
...
[info] Output written. Terminating...
```

**Expected files:**
- `output/MD_0000.vtk` (shows equilibrated liquid + disc at top)
- `output/MD_0200.vtk` (disc falling)
- `output/MD_0400.vtk` (disc impacting)
- ... (every 200 iterations)

### Phase 3 (Optional): Periodic Boundaries

Same as Phase 2, but use:
```bash
./build/MolSim input/drop-simulation-periodic.yml
```

## Verification

### Check Checkpoint File

```bash
# Linux/Mac
head -20 output/equilibrated.chkpt

# Windows
type output\equilibrated.chkpt | more
```

Should show:
```
# MolSim Checkpoint File
# Format: x y z vx vy vz m fx fy fz old_fx old_fy old_fz old_x old_y old_z p_id
# Particle count: 12500
1.5 2.0 0.0 0.123... 0.456... 0.0 1.0 ...
...
```

### Count Output Files

```bash
# Linux/Mac
ls -l output/MD_*.vtk | wc -l

# Windows
dir /b output\MD_*.vtk | find /c /v ""
```

**Equilibration:** Should have ~31 files (iterations 0-30000, every 1000)
**Main simulation:** Should have ~201 files (iterations 0-80000, every 200)

### Check Simulation Progress

Watch the log output for:
- ✅ "Loaded ... particles"
- ✅ "Iteration ... finished"
- ✅ "Checkpoint saved to: ..."
- ✅ No error messages

## Common Issues

### CMake can't find packages

**Problem:** `Could NOT find yaml-cpp` or similar

**Solution:**
1. Install dependencies (see main README.md)
2. Or specify paths:
   ```bash
   cmake -DYAML_CPP_DIR=/path/to/yaml-cpp ..
   ```

### Executable not found

**Problem:** `./build/MolSim: No such file or directory`

**Solution:**
1. Check build succeeded: `echo $?` (should be 0)
2. Look for executable:
   ```bash
   find build -name "MolSim*" -type f
   ```
3. May be in `build/Release/` or `build/Debug/`

### Checkpoint not found

**Problem:** `could not open checkpoint file: output/equilibrated.chkpt`

**Solution:**
1. Run Phase 1 (equilibration) first
2. Check file exists: `ls -l output/equilibrated.chkpt`
3. Check path in YAML is correct

### Particles escape domain

**Problem:** Simulation ends with 0 particles

**Solution:**
1. Check boundaries are "reflect" not "outflow"
2. Reduce time step if particles move too fast
3. Check domain size is large enough

### Simulation very slow

**Problem:** Takes hours to complete

**Solution:**
1. Reduce particle count (modify `n:` in YAML)
2. Increase `output_interval`
3. Use smaller `total_time` for testing
4. Enable optimizations: `cmake -DCMAKE_BUILD_TYPE=Release ..`

## Performance Expectations

On a modern CPU (2023+):

| Phase | Particles | Time Steps | Expected Time |
|-------|-----------|------------|---------------|
| Equilibration | 12,500 | 30,000 | ~5-15 minutes |
| Main Sim | 13,756 | 80,000 | ~15-30 minutes |

*Times vary based on CPU, compiler optimizations, and system load.*

## Debugging

### Enable verbose logging

Edit config YAML:
```yaml
config:
  log_level: debug  # Add this line
  ...
```

Or run with debug flag (if implemented):
```bash
./build/MolSim --log-level debug input/drop-equilibration.yml
```

### Check particle positions

Print checkpoint data:
```bash
# Show first 10 particles
head -13 output/equilibrated.chkpt | tail -10
```

Each line: `x y z vx vy vz m fx fy fz old_fx old_fy old_fz old_x old_y old_z p_id`

### Visualize immediately

Don't wait for full simulation:
1. Stop simulation early (Ctrl+C)
2. Open latest VTK file in ParaView
3. Check if particles look reasonable

## Next Steps

After successful runs:
1. Visualize in ParaView
2. Compare reflective vs periodic boundaries
3. Try different drop positions
4. Modify parameters and re-run
5. Analyze results

See [DROP_SIMULATION_IMPLEMENTATION.md](DROP_SIMULATION_IMPLEMENTATION.md) for detailed analysis.
