#!/usr/bin/env python3

"""
MolSim Task 2 - Benchmark Results Plotter
Generates strong-scaling speedup and efficiency plots
"""

import sys
import csv
import matplotlib.pyplot as plt
import numpy as np
from pathlib import Path
from collections import defaultdict

def read_csv(csv_file):
    """Read benchmark CSV and compute averages"""
    data = defaultdict(lambda: {'threads': [], 'times': [], 'speedups': []})
    
    with open(csv_file, 'r') as f:
        reader = csv.DictReader(f)
        for row in reader:
            strategy = int(row['Strategy'])
            threads = int(row['Threads'])
            elapsed = float(row['ExecutionTime_s'])
            
            key = strategy
            if threads not in data[key]['threads']:
                data[key]['threads'].append(threads)
            
            data[key]['times'].append(elapsed)
    
    # Compute averages and speedup
    result = {}
    for strategy, times_data in data.items():
        threads_list = sorted(set(data[strategy]['threads']))
        times_list = data[strategy]['times']
        
        # Group times by thread count
        times_by_thread = defaultdict(list)
        idx = 0
        for t in data[strategy]['threads']:
            times_by_thread[t].append(times_list[idx])
            idx += 1
        
        avg_times = []
        speedups = []
        baseline = None
        
        for threads in threads_list:
            avg_time = np.mean(times_by_thread[threads])
            avg_times.append(avg_time)
            
            if threads == 1:
                baseline = avg_time
                speedups.append(1.0)
            else:
                if baseline:
                    speedups.append(baseline / avg_time)
                else:
                    speedups.append(1.0)
        
        result[strategy] = {
            'threads': threads_list,
            'times': avg_times,
            'speedups': speedups
        }
    
    return result

def plot_results(data, output_file):
    """Generate speedup and efficiency plots"""
    
    fig, axes = plt.subplots(1, 2, figsize=(14, 5))
    
    strategy_names = {0: 'Strategy 0: Direct (O(n²))', 1: 'Strategy 1: Cell-based (O(n))'}
    colors = {0: '#2E86AB', 1: '#A23B72'}
    
    for strategy in sorted(data.keys()):
        threads = data[strategy]['threads']
        speedups = data[strategy]['speedups']
        times = data[strategy]['times']
        
        # Plot 1: Speedup curve
        axes[0].plot(threads, speedups, 'o-', label=strategy_names[strategy],
                    color=colors[strategy], linewidth=2.5, markersize=8)
        
        # Plot 2: Efficiency
        efficiency = [s / t * 100 for s, t in zip(speedups, threads)]
        axes[1].plot(threads, efficiency, 's-', label=strategy_names[strategy],
                    color=colors[strategy], linewidth=2.5, markersize=8)
    
    # Ideal speedup line
    max_threads = max(max(data[s]['threads']) for s in data)
    ideal_line = np.linspace(1, max_threads, 100)
    axes[0].plot(ideal_line, ideal_line, 'k--', label='Ideal (linear)', alpha=0.5, linewidth=1.5)
    
    # Ideal efficiency line
    axes[1].axhline(y=100, color='k', linestyle='--', label='Ideal (100%)', alpha=0.5, linewidth=1.5)
    
    # Configure plot 1: Speedup
    axes[0].set_xlabel('Number of Threads', fontsize=12, fontweight='bold')
    axes[0].set_ylabel('Speedup S(n) = T(1)/T(n)', fontsize=12, fontweight='bold')
    axes[0].set_title('Strong-Scaling Speedup\nTask 2: 2D MD Simulation (300×54)', 
                     fontsize=13, fontweight='bold')
    axes[0].legend(loc='upper left', fontsize=10)
    axes[0].grid(True, alpha=0.3, linestyle=':')
    axes[0].set_xlim([0.5, max_threads + 0.5])
    
    # Configure plot 2: Efficiency
    axes[1].set_xlabel('Number of Threads', fontsize=12, fontweight='bold')
    axes[1].set_ylabel('Parallel Efficiency (%)', fontsize=12, fontweight='bold')
    axes[1].set_title('Parallel Efficiency = S(n)/n × 100%\nTask 2: 2D MD Simulation (300×54)',
                     fontsize=13, fontweight='bold')
    axes[1].legend(loc='upper right', fontsize=10)
    axes[1].grid(True, alpha=0.3, linestyle=':')
    axes[1].set_ylim([0, 120])
    axes[1].set_xlim([0.5, max_threads + 0.5])
    
    plt.tight_layout()
    plt.savefig(output_file, dpi=300, bbox_inches='tight')
    print(f"✓ Plot saved: {output_file}")
    plt.close()

def generate_report(data, output_file):
    """Generate text report with statistics"""
    
    with open(output_file, 'w') as f:
        f.write("="*70 + "\n")
        f.write("MolSim Task 2 - Parallelization Benchmark Analysis\n")
        f.write("="*70 + "\n\n")
        
        f.write("Simulation Parameters:\n")
        f.write("  Domain: 300 × 54 (2D)\n")
        f.write("  Particles: 250×20 + 250×20 = 10,000 total\n")
        f.write("  Time: dt=0.0005, tend=50 sec\n")
        f.write("  Physics: Rayleigh-Taylor instability with gravity\n\n")
        
        for strategy in sorted(data.keys()):
            threads = np.array(data[strategy]['threads'])
            speedups = np.array(data[strategy]['speedups'])
            times = np.array(data[strategy]['times'])
            efficiency = speedups / threads * 100
            
            strategy_name = ['Direct (O(n²))', 'Cell-based (O(n))'][strategy]
            f.write(f"\n{'='*70}\n")
            f.write(f"Strategy {strategy}: {strategy_name}\n")
            f.write(f"{'='*70}\n\n")
            
            f.write(f"{'Threads':<8} {'Time(s)':<12} {'Speedup':<10} {'Efficiency':<12} {'Status':<12}\n")
            f.write("-"*70 + "\n")
            
            for t, time, speedup, eff in zip(threads, times, speedups, efficiency):
                if eff >= 90:
                    status = "Excellent"
                elif eff >= 75:
                    status = "Good"
                elif eff >= 50:
                    status = "Fair"
                else:
                    status = "Poor"
                
                f.write(f"{int(t):<8} {time:<12.3f} {speedup:<10.2f} {eff:<12.1f}% {status:<12}\n")
            
            # Statistics
            f.write("\nStatistics:\n")
            f.write(f"  Max speedup: {np.max(speedups):.2f}x at {threads[np.argmax(speedups)]:.0f} threads\n")
            f.write(f"  Avg efficiency: {np.mean(efficiency):.1f}%\n")
            f.write(f"  Min efficiency: {np.min(efficiency):.1f}%\n")
            f.write(f"  Max efficiency: {np.max(efficiency):.1f}%\n")
            
            # Scaling analysis
            f.write("\nScaling Analysis:\n")
            if len(threads) >= 2:
                # Linear regression for speedup curve
                coeffs = np.polyfit(threads[1:], speedups[1:], 1)  # Skip first point (T=1)
                slope = coeffs[0]
                if slope > 0.9:
                    f.write(f"  Scaling: SUPER-LINEAR (slope={slope:.3f})\n")
                elif slope > 0.5:
                    f.write(f"  Scaling: STRONG (slope={slope:.3f})\n")
                else:
                    f.write(f"  Scaling: WEAK (slope={slope:.3f})\n")
            
            f.write("\n")
    
    print(f"✓ Report saved: {output_file}")

def main():
    if len(sys.argv) < 2:
        print("Usage: python3 plot_results.py <csv_file> [output_dir]")
        print("Example: python3 plot_results.py results/benchmark_results.csv results/")
        sys.exit(1)
    
    csv_file = Path(sys.argv[1])
    output_dir = Path(sys.argv[2]) if len(sys.argv) > 2 else csv_file.parent
    
    if not csv_file.exists():
        print(f"✗ Error: {csv_file} not found")
        sys.exit(1)
    
    output_dir.mkdir(parents=True, exist_ok=True)
    
    print("Reading benchmark results...")
    data = read_csv(str(csv_file))
    
    if not data:
        print("✗ No data found in CSV file")
        sys.exit(1)
    
    print("Generating plots...")
    plot_file = output_dir / "speedup_curve.png"
    plot_results(data, str(plot_file))
    
    print("Generating report...")
    report_file = output_dir / "analysis.txt"
    generate_report(data, str(report_file))
    
    print(f"\n✓ Analysis complete!")
    print(f"Output directory: {output_dir}")

if __name__ == "__main__":
    main()

