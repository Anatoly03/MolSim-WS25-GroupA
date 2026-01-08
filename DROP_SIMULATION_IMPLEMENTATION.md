# Drop Simulation Implementation Summary

## Task Overview
Simulate a drop falling into a basin filled with liquid, including:
1. **Equilibration phase**: Let gravity act on the fluid until it arranges naturally
2. **Main simulation**: Add a drop (disc) and observe it falling into the equilibrated liquid
3. **Optional**: Test with periodic boundaries

## Implementation Components

### 1. Checkpointing System ✅

#### CheckpointWriter ([src/core/writer/CheckpointWriter.h](src/core/writer/CheckpointWriter.h))
- Saves complete particle state to `.chkpt` files
- Includes all properties needed for exact restart:
  - Position (x, y, z)
  - Velocity (vx, vy, vz)
  - Mass (m)
  - Current force (fx, fy, fz)
  - Old force (old_fx, old_fy, old_fz)
  - Old position (old_x, old_y, old_z)
  - Particle ID (p_id)
- Uses high precision (17 digits) for numerical accuracy
- Format is human-readable and includes header comments

#### CheckpointReader ([src/core/reader/CheckpointReader.h](src/core/reader/CheckpointReader.h))
- Reads `.chkpt` files and restores complete particle state
- Integrated with FileReader factory for automatic format detection
- Validates input and reports errors clearly

#### Integration
- Modified [FileReader.cpp](src/core/reader/FileReader.cpp) to recognize `.chkpt` extension
- Modified [YamlReader.h](src/core/reader/YamlReader.h) to support `checkpoint_input` parameter
- Modified [MolSim.cpp](src/app/MolSim.cpp) to save checkpoint at end if `checkpoint_path` is specified

### 2. Gravity Support ✅

#### Implementation
- Added `gravity_force` field to [Args.h](src/core/utils/Args.h) as Vec3D
- Modified [Simulation.cpp](src/core/simulation/Simulation.cpp) `calculateSingleVelocity()`:
  - Adds gravitational force to total force: `F_total = F_particles + m * g`
  - Properly averaged with old forces for Verlet integration
- Configured via YAML: `gravity_force: [0.0, -12.44, 0.0]`

### 3. Thermostat ✅

#### Implementation
- Added thermostat parameters to [Args.h](src/core/utils/Args.h):
  - `thermostat_interval`: How often to apply (0 = disabled)
  - `thermostat_temperature`: Target temperature
  - `initial_temperature`: For particle generation
- Added methods to [Simulation.h/cpp](src/core/simulation/Simulation.cpp):
  - `calculateKineticEnergy()`: Computes total kinetic energy
  - `applyThermostat()`: Maxwell-Boltzmann velocity rescaling
- Integrated into [Simulation.h](src/core/simulation/Simulation.h) `run()` method
- Temperature calculation: T = (2 * E_kin) / (3 * N) for 3D systems
- Velocity scaling: β = sqrt(T_target / T_current)

#### Algorithm
```
For every thermostat_interval iterations:
  1. Calculate current kinetic energy: E_kin = Σ(0.5 * m * v²)
  2. Calculate current temperature: T_current = (2 * E_kin) / (3 * N)
  3. Calculate scaling factor: β = sqrt(T_target / T_current)
  4. Scale all velocities: v_new = β * v_old
```

### 4. Configuration Files ✅

#### Equilibration ([input/drop-equilibration.yml](input/drop-equilibration.yml))
```yaml
# Phase 1: Equilibration
- Duration: 15 time units
- Delta t: 0.0005
- Gravity: [0, -12.44, 0]
- Thermostat: ON (interval=1000, T=0.5)
- Domain: [303, 180, 1]
- Liquid: 250×50 particles
- All boundaries: reflective
- Saves checkpoint: output/equilibrated.chkpt
```

#### Main Simulation ([input/drop-simulation.yml](input/drop-simulation.yml))
```yaml
# Phase 2: Drop simulation
- Duration: 40 time units
- Delta t: 0.0005
- Gravity: [0, -12.44, 0]
- Thermostat: OFF
- Loads checkpoint: output/equilibrated.chkpt
- Adds disc: center=[150, 150], radius=20
- All boundaries: reflective
```

#### Periodic Boundaries ([input/drop-simulation-periodic.yml](input/drop-simulation-periodic.yml))
```yaml
# Optional: Periodic boundaries
- Same as main simulation
- Left/Right boundaries: periodic
- Top/Bottom boundaries: reflective
- Different drop position: [75, 150]
```

## How to Use

### Step 1: Build the Project

**Linux/Mac:**
```bash
chmod +x build-and-run.sh
./build-and-run.sh
```

**Windows:**
```cmd
build-and-run.bat
```

Or manually:
```bash
mkdir -p build && cd build
cmake ..
make
cd ..
```

### Step 2: Run Equilibration

```bash
./build/MolSim input/drop-equilibration.yml
```

This will:
- Simulate 15 time units with gravity and thermostat
- Create ~12,500 particles (250×50)
- Apply thermostat every 1000 iterations to maintain T=0.5
- Save checkpoint to `output/equilibrated.chkpt`
- Generate VTK/XYZ output files in `output/`

### Step 3: Run Main Simulation

```bash
./build/MolSim input/drop-simulation.yml
```

This will:
- Load equilibrated fluid from checkpoint
- Add disc with ~1,256 particles (radius 20, spacing 1.2)
- Simulate 40 time units with gravity (no thermostat)
- Disc falls and splashes into liquid
- Generate output files showing the interaction

### Step 4 (Optional): Try Periodic Boundaries

```bash
./build/MolSim input/drop-simulation-periodic.yml
```

**Expected differences:**
- Particles can wrap around left/right edges
- Different splash pattern due to periodic BC
- Drop starts at different position (x=75 vs x=150)
- May observe periodic wave propagation

## Physics Parameters

### Equilibration Phase
| Parameter | Value | Description |
|-----------|-------|-------------|
| g_grav | -12.44 | Gravitational acceleration (y-direction) |
| Δt | 0.0005 | Time step size |
| t_end | 15 | Total simulation time |
| L | {303, 180} | Domain size |
| T_init | 0.5 | Initial temperature |
| r_cutoff | 3.0 | Cutoff radius (2.5σ) |
| n_thermostat | 1000 | Thermostat application interval |

### Liquid Properties
| Parameter | Value | Description |
|-----------|-------|-------------|
| x_l | {1.5, 2} | Initial position |
| n_l | {250, 50} | Number of particles |
| h | 1.2 | Particle spacing |
| m | 1.0 | Mass |
| ε | 1.0 | Lennard-Jones epsilon |
| σ | 1.2 | Lennard-Jones sigma |
| v | {0, 0} | Initial velocity |

### Drop Properties
| Parameter | Value | Description |
|-----------|-------|-------------|
| x_s | {150, 150} | Center position |
| R_s | 20 | Radius |
| Same as liquid | - | Mass, ε, σ, h |

## Technical Notes

### Numerical Integration
- Uses Störmer-Verlet integration scheme
- Position update: x(t+Δt) = x(t) + v(t)Δt + F(t)Δt²/(2m)
- Velocity update: v(t+Δt) = v(t) + [F(t) + F(t+Δt) + 2mg]Δt/(2m)
- Gravity is added to forces during velocity update

### Memory Considerations
- Equilibration: ~12,500 particles
- Main simulation: ~13,750 particles (12,500 + 1,256)
- Checkpoint file size: ~1MB per 10,000 particles

### Performance
- Linked cell algorithm: O(N) complexity
- Cell size: 3×3×3 (optimal for cutoff radius 3.0)
- Domain divided into ~100×60×1 cells

## Files Modified/Created

### New Files
- `src/core/writer/CheckpointWriter.h` - Checkpoint output
- `src/core/reader/CheckpointReader.h` - Checkpoint input
- `input/drop-equilibration.yml` - Equilibration config
- `input/drop-simulation.yml` - Main simulation config
- `input/drop-simulation-periodic.yml` - Periodic BC variant
- `input/DROP_SIMULATION_README.md` - Detailed instructions
- `build-and-run.sh` - Build script (Linux/Mac)
- `build-and-run.bat` - Build script (Windows)

### Modified Files
- `src/core/utils/Args.h` - Added gravity, thermostat, checkpoint parameters
- `src/core/reader/YamlReader.h` - Added parameter reading + checkpoint_input support
- `src/core/reader/FileReader.cpp` - Added .chkpt format support
- `src/core/simulation/Simulation.h` - Added thermostat methods
- `src/core/simulation/Simulation.cpp` - Implemented gravity and thermostat
- `src/app/MolSim.cpp` - Added checkpoint saving at end

## Verification

### Expected Behavior

**Equilibration:**
- Particles start at z=0 (2D simulation)
- Fall under gravity and settle at bottom
- Temperature oscillates around 0.5
- System reaches equilibrium by t=15

**Main Simulation:**
- Disc falls from top (y=150)
- Accelerates under gravity
- Impacts liquid surface
- Creates splash and waves
- Particles interact via Lennard-Jones force

**Periodic Boundaries:**
- Similar to main simulation
- Particles can wrap horizontally
- Different interaction pattern
- Drop at different starting position may show different splash

## Troubleshooting

### Checkpoint file not found
- Ensure equilibration completed successfully
- Check `output/` directory exists
- Verify path in `checkpoint_input` is correct

### Simulation too slow
- Reduce particle count (modify n_l in config)
- Increase output_interval
- Use smaller domain

### Particles flying away
- Check gravity direction (should be negative y)
- Verify boundary conditions are reflective
- Check time step is not too large

### Temperature not stable
- Increase thermostat_interval for less frequent application
- Check initial_temperature matches thermostat_temperature
- Verify brownian_sigma is small or zero

## Future Enhancements

Possible extensions:
1. Add particle type support for different liquids
2. Implement smooth thermostat (Nosé-Hoover)
3. Support 3D spheres (balls) as drops
4. Add visualization of temperature field
5. Checkpoint at multiple time points
6. Resume simulation from checkpoint (continue, not restart)

## References

- Lennard-Jones potential
- Störmer-Verlet integration
- Maxwell-Boltzmann distribution
- Velocity rescaling thermostat
