#!/bin/bash
# Script to build and run the drop simulation

echo "============================================"
echo "Drop Simulation Build and Run Script"
echo "============================================"

# Create build directory if it doesn't exist
if [ ! -d "build" ]; then
    echo "Creating build directory..."
    mkdir build
fi

cd build

# Configure with CMake
echo ""
echo "Configuring with CMake..."
cmake .. || { echo "CMake configuration failed!"; exit 1; }

# Build
echo ""
echo "Building project..."
make -j$(nproc) || { echo "Build failed!"; exit 1; }

cd ..

# Create output directory if it doesn't exist
if [ ! -d "output" ]; then
    echo ""
    echo "Creating output directory..."
    mkdir output
fi

echo ""
echo "============================================"
echo "Build successful!"
echo "============================================"
echo ""
echo "To run the simulations:"
echo ""
echo "1. Equilibration phase (creates checkpoint):"
echo "   ./build/MolSim input/drop-equilibration.yml"
echo ""
echo "2. Main simulation (uses checkpoint + adds drop):"
echo "   ./build/MolSim input/drop-simulation.yml"
echo ""
echo "3. Optional: Periodic boundary version:"
echo "   ./build/MolSim input/drop-simulation-periodic.yml"
echo ""
echo "============================================"
