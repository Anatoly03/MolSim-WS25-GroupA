#!/bin/bash

##############################################################################
# MolSim Task 2 - Parallelization Benchmark
# Strong-scaling performance test for 2D molecular dynamics
#
# Configuration: Worksheet 5, Task 2 requirements
# - 2D system: 300 x 54
# - Two liquid components with different sigma values
# - Testing strategies: 0 (direct), 1 (cell-based)
# - Thread counts: 1, 2, 4, 8, 14, 16
#
# Usage: ./run_benchmark.sh [num_runs]
#        ./run_benchmark.sh 3    # Run each config 3 times
##############################################################################

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$PROJECT_ROOT/build"
EXECUTABLE="$BUILD_DIR/MolSim"
CONFIG_FILE="$SCRIPT_DIR/benchmark_test_config.yml"
OUTPUT_DIR="$SCRIPT_DIR/test_results"
RESULTS_DIR="$SCRIPT_DIR/results"

# Configuration
THREAD_COUNTS=(1 2 4 8 14 16)
STRATEGIES=(0 1)  # 0=Direct, 1=Cell-based
NUM_RUNS=${1:-2}  # Number of repetitions per config

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

# Setup environment
export LDFLAGS="-L/opt/homebrew/opt/libomp/lib"
export CPPFLAGS="-I/opt/homebrew/opt/libomp/include"
export CC=/opt/homebrew/opt/llvm/bin/clang
export CXX=/opt/homebrew/opt/llvm/bin/clang++

# Verify setup
if [[ ! -f "$EXECUTABLE" ]]; then
    echo -e "${RED}✗ Error: MolSim not found at $EXECUTABLE${NC}"
    echo "Please build first: cd $PROJECT_ROOT/build && cmake -DENABLE_OPENMP=ON .. && make"
    exit 1
fi

if [[ ! -f "$CONFIG_FILE" ]]; then
    echo -e "${RED}✗ Error: Config file not found at $CONFIG_FILE${NC}"
    exit 1
fi

# Setup directories
mkdir -p "$OUTPUT_DIR" "$RESULTS_DIR"
rm -f "$OUTPUT_DIR"/*.vtu  # Clean old results

echo -e "${BLUE}========== Task 2: Parallelization Benchmark ==========${NC}"
echo "Project: MolSim - 2D Molecular Dynamics"
echo "Build: $EXECUTABLE"
echo "Config: $CONFIG_FILE"
echo "Output: $OUTPUT_DIR"
echo "Thread counts: ${THREAD_COUNTS[@]}"
echo "Strategies: ${STRATEGIES[@]} (0=Direct O(n²), 1=Cell-based O(n))"
echo "Runs per config: $NUM_RUNS"
echo ""

# Results file header
RESULTS_CSV="$RESULTS_DIR/benchmark_results.csv"
cat > "$RESULTS_CSV" << EOF
Strategy,Threads,Run,ExecutionTime_s,Speedup
EOF

declare -A BASELINE  # Store baseline times for speedup calculation

# Main benchmark loop
for strategy in "${STRATEGIES[@]}"; do
    STRATEGY_NAME=$([ $strategy -eq 0 ] && echo "Direct (O(n²))" || echo "Cell-based (O(n))")
    echo -e "${YELLOW}Strategy $strategy: $STRATEGY_NAME${NC}"
    
    # Create strategy-specific config
    TEMP_CONFIG="$OUTPUT_DIR/config_s${strategy}.yml"
    cp "$CONFIG_FILE" "$TEMP_CONFIG"
    sed -i '' "s/parallelization_strategy: .*/parallelization_strategy: $strategy/" "$TEMP_CONFIG"
    
    for threads in "${THREAD_COUNTS[@]}"; do
        echo -n "  Threads=$threads: "
        
        for run in $(seq 1 $NUM_RUNS); do
            export OMP_NUM_THREADS=$threads
            
            # Run simulation
            START_TIME=$(date +%s%N)
            "$EXECUTABLE" "$TEMP_CONFIG" > /dev/null 2>&1 || true
            END_TIME=$(date +%s%N)
            
            # Calculate elapsed time in seconds
            ELAPSED_NS=$((END_TIME - START_TIME))
            ELAPSED_SEC=$(echo "scale=3; $ELAPSED_NS / 1000000000" | bc)
            
            # Calculate speedup
            KEY="strategy_${strategy}"
            if [[ $threads -eq 1 ]]; then
                # Store baseline for first thread
                if [[ -z "${BASELINE[$KEY]}" ]]; then
                    BASELINE[$KEY]=$ELAPSED_SEC
                fi
                SPEEDUP=1.0
            else
                SPEEDUP=$(echo "scale=2; ${BASELINE[$KEY]} / $ELAPSED_SEC" | bc)
            fi
            
            # Log result
            echo "$strategy,$threads,$run,$ELAPSED_SEC,$SPEEDUP" >> "$RESULTS_CSV"
            
            echo -n "[$run:${ELAPSED_SEC}s] "
        done
        
        echo "✓"
    done
    
    rm -f "$TEMP_CONFIG"
    echo ""
done

# Generate summary report
REPORT="$RESULTS_DIR/benchmark_report.txt"
cat > "$REPORT" << EOF
MolSim Task 2 - Parallelization Benchmark Report
Generated: $(date)

Configuration:
  - Scenario: 2D Rayleigh-Taylor instability simulation
  - Domain: 300 × 54
  - dt = 0.0005, tend = 50 seconds
  - g = -12.44, T = 40K, t_thermostat = 1000
  - Liquid 1: 250×20 particles, σ₁=1.2, m₁=1.0
  - Liquid 2: 250×20 particles, σ₂=1.1, m₂=2.0

Performance Results:
  - Baseline: T(1 thread)
  - Speedup: S(n) = T(1) / T(n)
  - Efficiency: E(n) = S(n) / n × 100%

Thread Counts Tested: ${THREAD_COUNTS[@]}
Strategies:
  - Strategy 0: Direct parallelization (all pairs, O(n²))
  - Strategy 1: Cell-based (spatial partitioning, O(n))

Raw Results: $RESULTS_CSV

EOF

echo -e "${GREEN}✓ Benchmark completed!${NC}"
echo "Results saved to:"
echo "  CSV: $RESULTS_CSV"
echo "  Report: $REPORT"
echo ""
echo "Next steps:"
echo "  1. Generate plots: python3 plot_results.py $RESULTS_CSV"
echo "  2. View results: cat $RESULTS_CSV"
echo "  3. VTK output: ls -lh $OUTPUT_DIR/*.vtu"
echo ""

