#!/usr/bin/env python3
"""
Compare two XYZ frames to verify particle motion.
Usage: python scripts/compare_frames.py output/test_fix_0000.xyz output/test_fix_0010.xyz
"""

import sys

def read_xyz(filename):
    """Read an XYZ file and return list of particle positions."""
    particles = []
    with open(filename, 'r') as f:
        n = int(f.readline().strip())
        f.readline()  # skip comment
        for i in range(n):
            parts = f.readline().strip().split()
            x, y, z = float(parts[1]), float(parts[2]), float(parts[3])
            particles.append((x, y, z))
    return particles

def compare_frames(file1, file2):
    """Compare two frames and print statistics."""
    p1 = read_xyz(file1)
    p2 = read_xyz(file2)
    
    if len(p1) != len(p2):
        print(f"ERROR: Different number of particles: {len(p1)} vs {len(p2)}")
        return
    
    n = len(p1)
    print(f"Comparing {n} particles")
    print(f"Frame 1: {file1}")
    print(f"Frame 2: {file2}")
    print()
    
    # Calculate displacements
    total_disp = 0.0
    max_disp = 0.0
    max_disp_idx = -1
    y_disp_sum = 0.0  # sum of Y displacements (gravity direction)
    
    for i in range(min(n, 10)):  # print first 10 particles
        dx = p2[i][0] - p1[i][0]
        dy = p2[i][1] - p1[i][1]
        dz = p2[i][2] - p1[i][2]
        disp = (dx**2 + dy**2 + dz**2)**0.5
        print(f"Particle {i:4d}: pos1=({p1[i][0]:8.4f}, {p1[i][1]:8.4f}, {p1[i][2]:8.4f}) "
              f"pos2=({p2[i][0]:8.4f}, {p2[i][1]:8.4f}, {p2[i][2]:8.4f}) "
              f"disp={disp:.6f} (dy={dy:.6f})")
    
    print()
    print("Statistics for all particles:")
    for i in range(n):
        dx = p2[i][0] - p1[i][0]
        dy = p2[i][1] - p1[i][1]
        dz = p2[i][2] - p1[i][2]
        disp = (dx**2 + dy**2 + dz**2)**0.5
        total_disp += disp
        y_disp_sum += dy
        if disp > max_disp:
            max_disp = disp
            max_disp_idx = i
    
    avg_disp = total_disp / n
    avg_y_disp = y_disp_sum / n
    
    print(f"  Average displacement: {avg_disp:.6f}")
    print(f"  Max displacement:     {max_disp:.6f} (particle {max_disp_idx})")
    print(f"  Average Y displacement: {avg_y_disp:.6f}")
    
    if max_disp < 1e-10:
        print("\n⚠️  WARNING: Particles have not moved! Check force calculation.")
        return False
    else:
        print(f"\n✅ Particles moved successfully (max disp = {max_disp:.6f})")
        return True

if __name__ == '__main__':
    if len(sys.argv) != 3:
        print("Usage: python compare_frames.py <file1.xyz> <file2.xyz>")
        sys.exit(1)
    
    compare_frames(sys.argv[1], sys.argv[2])
