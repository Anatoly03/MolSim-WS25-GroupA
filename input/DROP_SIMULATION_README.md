# Drop Simulation Task

This folder contains configuration files for simulating a drop falling into a basin filled with liquid.

## Simulation Workflow

### Phase 1: Equilibration (Gravity + Thermostat)

Run the equilibration phase to let the fluid arrange naturally:

```bash
./MolSim input/drop-equilibration.yml
```

**Parameters:**
- Gravity: g_grav = -12.44 (in y-direction)
- Time step: delta_t = 0.0005
- Duration: t_end = 15
- Domain size: L = {303, 180}
- Initial temperature: T_init = 0.5
- Cutoff radius: r_cutoff = 2.5σ = 3.0
- Thermostat interval: 1000 iterations
- All boundaries: reflective

**Liquid parameters:**
- Position: xl = {1.5, 2}
- Particle count: nl = {250, 50}
- Spacing: h = 1.2
- Mass: m = 1.0
- Epsilon: ε = 1.0
- Sigma: σ = 1.2
- Velocity: v = {0, 0}

This will save a checkpoint file to `output/equilibrated.chkpt`.

### Phase 2: Drop Simulation (No Thermostat)

Run the main simulation with the equilibrated fluid and a falling disc:

```bash
./MolSim input/drop-simulation.yml
```

**Parameters:**
- Uses equilibrated state from checkpoint
- Duration: t_end = 40
- Thermostat: OFF
- Adds a disc (2D sphere) as the drop:
  - Center: xs = {150, 150}
  - Radius: Rs = 20
  - Same material properties as liquid

### Optional: Periodic Boundaries

Try the simulation with periodic boundaries on left/right sides:

```bash
./MolSim input/drop-simulation-periodic.yml
```

**Differences:**
- Left and right boundaries are periodic
- Drop starts at different position (x = 75 instead of 150)
- Allows particles to wrap around horizontally

## Implementation Details

### Checkpointing

The implementation includes:

1. **CheckpointWriter** ([src/core/writer/CheckpointWriter.h](../src/core/writer/CheckpointWriter.h))
   - Saves complete particle state including:
     - Position, velocity, mass
     - Force and old_force
     - Old_position
     - Particle ID (p_id)
   - High precision (17 digits) for exact restart

2. **CheckpointReader** ([src/core/reader/CheckpointReader.h](../src/core/reader/CheckpointReader.h))
   - Restores complete particle state from checkpoint files
   - Supports `.chkpt` file extension
   - Integrated with FileReader factory

### Gravity Support

External forces (like gravity) are applied in the velocity calculation:
- Added `gravity_force` parameter to Args
- Applied as: F_total = F_particles + m * g
- Configured via YAML: `gravity_force: [0.0, -12.44, 0.0]`

### Thermostat

Maxwell-Boltzmann velocity rescaling thermostat:
- Applied every N iterations (configurable)
- Calculates current temperature from kinetic energy
- Scales velocities: v_new = v_old * sqrt(T_target / T_current)
- Can be disabled by setting `thermostat_interval: 0`

### YAML Configuration Extensions

New configuration parameters:
```yaml
config:
  gravity_force: [0.0, -12.44, 0.0]  # External force vector
  thermostat_interval: 1000           # Apply every N iterations (0=off)
  thermostat_temperature: 0.5         # Target temperature
  initial_temperature: 0.5            # For initial velocity generation
  checkpoint_path: "output/file.chkpt"  # Save checkpoint at end
  checkpoint_input: "output/file.chkpt" # Load checkpoint at start
```

## Expected Behavior

### Equilibration Phase
- Particles settle under gravity
- Thermostat maintains temperature ~0.5
- System reaches equilibrium after ~15 time units

### Main Simulation
- Disc falls due to gravity
- Splashes into the equilibrated liquid
- Creates waves and particle interactions
- Energy is conserved (no thermostat)

### Periodic Boundaries (Optional)
- Particles wrap around left/right edges
- May observe different splash patterns
- Drop position affects interaction with periodic boundaries

## Compilation

Make sure to recompile the project after adding the new files:

```bash
mkdir -p build
cd build
cmake ..
make
```

## Output

- Regular VTK/XYZ output files: `output/MD_<iteration>.[vtk|xyz]`
- Checkpoint file: `output/equilibrated.chkpt`
- View with ParaView or similar visualization tool
