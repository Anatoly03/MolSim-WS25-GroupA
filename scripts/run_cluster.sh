#!/bin/bash
#SBATCH --job-name=molsim_perf
#SBATCH --output=performance_%j.out
#SBATCH --error=performance_%j.err
#SBATCH --partition=cm4_inter
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=1
#SBATCH --time=01:00:00
#SBATCH --export=NONE

# Performance measurement script for MolSim on CoolMUC4
# Usage: sbatch scripts/run_cluster.sh

module load slurm_setup

echo "=== Build Information ==="
echo "Hostname: $(hostname)"
echo "Date: $(date)"
echo "Compiler: $(gcc --version | head -n1)"
echo "Build type: Release"
echo ""

cd $SLURM_SUBMIT_DIR

if [ ! -f build/MolSim.exe ]; then
    echo "ERROR: build/MolSim.exe not found!"
    exit 1
fi

INPUT_FILE="input/full_nl_250_50.txt"
T_END=1.0
DT=0.0005

echo "=== Performance Test Parameters ==="
echo "Input: $INPUT_FILE"
echo "Time: 0 -> $T_END (dt=$DT)"
echo "Particles: 12500"
echo "I/O: disabled"
echo ""

echo "=== Running Performance Test ==="
time ./build/MolSim.exe $INPUT_FILE \
    -t $T_END \
    -d $DT \
    -g -12.44 \
    --nthermostat 1000 \
    --Lx 303 \
    --Ly 180 \
    --sigma 1.2 \
    --epsilon 1.0 \
    --rcut 3.0 \
    --no-io

echo ""
echo "=== Job Complete ==="
