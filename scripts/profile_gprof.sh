#!/bin/bash
# Performance profiling script using gprof

set -e

echo "=== MolSim Performance Profiling with gprof ==="
echo "Date: $(date)"
echo ""

echo "Step 1: Building with gprof profiling..."
rm -rf build_prof
mkdir build_prof
cd build_prof
cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DENABLE_GPROF=ON ..
mingw32-make -j4
cd ..

echo "Build complete."
echo ""

echo "Step 2: Running performance test..."
./build_prof/MolSim.exe input/full_nl_250_50.txt \
    -t 1.0 \
    -d 0.0005 \
    -g -12.44 \
    --nthermostat 1000 \
    --Lx 303 \
    --Ly 180 \
    --sigma 1.2 \
    --epsilon 1.0 \
    --rcut 3.0 \
    --no-io 2>/dev/null | grep -E "Simulation loop time|MUPS"

echo ""

if [ -f gmon.out ]; then
    echo "Step 3: Generating profiling report..."
    gprof build_prof/MolSim.exe gmon.out > profiling_report.txt
    
    echo ""
    echo "=== Top 10 Functions by Time ==="
    grep -A 15 "^  %   cumulative" profiling_report.txt | head -n 20
    
    echo ""
    echo "Full report saved to: profiling_report.txt"
else
    echo "ERROR: gmon.out not found!"
    exit 1
fi

echo ""
echo "=== Profiling Complete ==="
