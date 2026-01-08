# Quick Start Guide - Drop Simulation

## 🚀 Quick Steps

### 1. Build
```bash
# Windows
build-and-run.bat

# Linux/Mac
chmod +x build-and-run.sh
./build-and-run.sh
```

### 2. Run Equilibration (Phase 1)
```bash
# Windows
build\Release\MolSim.exe input\drop-equilibration.yml

# Linux/Mac
./build/MolSim input/drop-equilibration.yml
```

**What it does:**
- Creates liquid basin with 12,500 particles
- Applies gravity (g = -12.44)
- Maintains temperature at T = 0.5 with thermostat
- Runs for 15 time units
- **Saves checkpoint:** `output/equilibrated.chkpt`

### 3. Run Main Simulation (Phase 2)
```bash
# Windows
build\Release\MolSim.exe input\drop-simulation.yml

# Linux/Mac
./build/MolSim input/drop-simulation.yml
```

**What it does:**
- **Loads** equilibrated liquid from checkpoint
- **Adds** falling disc (radius 20, ~1,256 particles)
- Disc falls and splashes into liquid
- Runs for 40 time units
- No thermostat (energy conservation)

### 4. (Optional) Try Periodic Boundaries
```bash
# Windows
build\Release\MolSim.exe input\drop-simulation-periodic.yml

# Linux/Mac
./build/MolSim input/drop-simulation-periodic.yml
```

**Differences:**
- Left/Right boundaries: **periodic** (particles wrap around)
- Drop starts at x=75 instead of x=150

## 📊 Output

- **VTK/XYZ files:** `output/MD_####.[vtk|xyz]`
- **Checkpoint:** `output/equilibrated.chkpt`
- **View with:** ParaView, VMD, or similar

## ⚙️ Key Parameters

| Parameter | Equilibration | Main Sim |
|-----------|---------------|----------|
| Duration | 15 | 40 |
| Δt | 0.0005 | 0.0005 |
| Gravity | -12.44 | -12.44 |
| Thermostat | ON (T=0.5) | OFF |
| Particles | 12,500 | 13,756 |

## 📝 Configuration Files

1. **[input/drop-equilibration.yml](input/drop-equilibration.yml)** - Phase 1
2. **[input/drop-simulation.yml](input/drop-simulation.yml)** - Phase 2
3. **[input/drop-simulation-periodic.yml](input/drop-simulation-periodic.yml)** - Optional

## 📚 Documentation

- **Full details:** [DROP_SIMULATION_IMPLEMENTATION.md](DROP_SIMULATION_IMPLEMENTATION.md)
- **Task instructions:** [input/DROP_SIMULATION_README.md](input/DROP_SIMULATION_README.md)

## ✅ What Was Implemented

- ✅ **Checkpointing** - Save/load complete simulation state
- ✅ **Gravity** - External force support
- ✅ **Thermostat** - Maxwell-Boltzmann velocity rescaling
- ✅ **Equilibration** - 15 time units with gravity + thermostat
- ✅ **Drop simulation** - Disc falling into liquid
- ✅ **Periodic boundaries** - Optional variant
- ✅ **Multiple molecule types** - Checkpoint supports any particle type

## 🔧 Troubleshooting

**"Checkpoint not found"**
→ Run equilibration first (Phase 1)

**"Simulation too slow"**
→ Increase `output_interval` in YAML config

**"Particles escaping"**
→ Check boundaries are "reflect" (not "outflow")

## 🎯 Expected Results

**Equilibration:**
- Particles settle at bottom under gravity
- Temperature stabilizes around 0.5
- Fluid reaches equilibrium

**Main Simulation:**
- Disc accelerates downward
- Impacts liquid surface
- Creates splash effect
- Waves propagate through liquid
- Energy is conserved (no thermostat)

---

**Ready to simulate!** 🌊💧
