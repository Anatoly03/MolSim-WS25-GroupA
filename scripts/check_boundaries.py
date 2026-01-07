#!/usr/bin/env python3
"""
Check if all particles are within domain boundaries.
Usage: python scripts/check_boundaries.py output/test_0010.xyz 303 180
"""

import sys

def check_boundaries(filename, Lx, Ly):
    """Check if all particles are within [0, Lx] x [0, Ly]."""
    with open(filename, 'r') as f:
        n = int(f.readline().strip())
        f.readline()  # skip comment
        
        out_of_bounds = 0
        x_min, x_max = float('inf'), float('-inf')
        y_min, y_max = float('inf'), float('-inf')
        
        for i in range(n):
            parts = f.readline().strip().split()
            x, y, z = float(parts[1]), float(parts[2]), float(parts[3])
            
            x_min = min(x_min, x)
            x_max = max(x_max, x)
            y_min = min(y_min, y)
            y_max = max(y_max, y)
            
            if x < 0 or x > Lx or y < 0 or y > Ly:
                out_of_bounds += 1
                if out_of_bounds <= 5:  # print first 5
                    print(f"  Particle {i}: x={x:.3f} y={y:.3f} (OUT OF BOUNDS)")
        
        print(f"\nFile: {filename}")
        print(f"Domain: [0, {Lx}] x [0, {Ly}]")
        print(f"Total particles: {n}")
        print(f"X range: [{x_min:.3f}, {x_max:.3f}]")
        print(f"Y range: [{y_min:.3f}, {y_max:.3f}]")
        print(f"Out of bounds: {out_of_bounds}")
        
        if out_of_bounds == 0:
            print("✅ All particles within boundaries!")
            return True
        else:
            print(f"⚠️  {out_of_bounds} particles escaped!")
            return False

if __name__ == '__main__':
    if len(sys.argv) != 4:
        print("Usage: python check_boundaries.py <file.xyz> <Lx> <Ly>")
        sys.exit(1)
    
    check_boundaries(sys.argv[1], float(sys.argv[2]), float(sys.argv[3]))
