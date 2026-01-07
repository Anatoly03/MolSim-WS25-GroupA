#!/bin/bash
#SBATCH --job-name=molsim_prof
#SBATCH --output=profile_%j.out
#SBATCH --error=profile_%j.err
#SBATCH --partition=cm4_inter
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=1
#SBATCH --time=00:30:00
#SBATCH --export=NONE

# Profiling script for MolSim on CoolMUC4 using gprof
# Usage: sbatch scripts/run_cluster_profile.sh

module load slurm_setup

echo "=== Build Information ==="
echo "Hostname: $(hostname)"
echo "Date: $(date)"
echo "Compiler: $(gcc --version | head -n1)"
echo "Build type: Release with gprof (-pg)"
echo ""

cd $SLURM_SUBMIT_DIR

if [ ! -f build_prof/MolSim.exe ]; then
    echo "ERROR: build_prof/MolSim.exe not found!"
    echo "Please build with: cmake -Bbuild_prof -DCMAKE_BUILD_TYPE=Release -DENABLE_GPROF=ON && cmake --build build_prof -- -j4"
    exit 1
fi

INPUT_FILE="input/full_nl_250_50.txt"
T_END=1.0
DT=0.0005

echo "=== Profiling Test Parameters ==="
echo "Input: $INPUT_FILE"
echo "Time: 0 -> $T_END (dt=$DT)"
echo "Particles: 12500"
echo "I/O: disabled"
echo ""

echo "=== Running with gprof profiling ==="
time ./build_prof/MolSim.exe $INPUT_FILE \
    -t $T_END \
    -d $DT \
    -g -12.44 \
    -N 1000 \
    --Lx 303 \
    --Ly 180 \
    -s 1.2 \
    -e 1.0 \
    -r 3.0 \
    -i

echo ""

if [ -f gmon.out ]; then
    echo "=== Generating gprof report ==="
    gprof build_prof/MolSim.exe gmon.out > profiling_report_${SLURM_JOB_ID}.txt
    
    echo ""
    echo "=== Top 10 Functions by Time ==="
    grep -A 15 "^  %   cumulative" profiling_report_${SLURM_JOB_ID}.txt | head -n 20
    
    echo ""
    echo "Full profiling report saved to: profiling_report_${SLURM_JOB_ID}.txt"
else
    echo "ERROR: gmon.out not found! Profiling data was not generated."
    exit 1
fi

echo ""
echo "=== Profiling Complete ==="
echo "Job ID: $SLURM_JOB_ID"
