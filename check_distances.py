import sys
import numpy as np
from scipy.spatial import KDTree

def read_xyz(filename):
    """读取XYZ文件"""
    with open(filename, 'r') as f:
        num_particles = int(f.readline().strip())
        f.readline()  # 跳过注释行
        positions = []
        for line in f:
            parts = line.strip().split()
            if len(parts) >= 4:
                x, y, z = float(parts[1]), float(parts[2]), float(parts[3])
                positions.append([x, y, z])
    return np.array(positions)

def analyze_distances(positions, sigma=1.2):
    """分析粒子间距"""
    tree = KDTree(positions)
    
    # 对每个粒子找最近邻
    distances, indices = tree.query(positions, k=2)  # k=2因为第一个是自己
    nearest_distances = distances[:, 1]  # 取第二个（最近邻）
    
    # LJ平衡距离 r_eq = 2^(1/6) * σ
    r_eq = 2**(1/6) * sigma
    
    print(f"=== 粒子间距分析 ===")
    print(f"总粒子数: {len(positions)}")
    print(f"\nLennard-Jones 参数:")
    print(f"  σ = {sigma}")
    print(f"  平衡距离 r_eq = 2^(1/6)×σ = {r_eq:.4f}")
    print(f"  最小能量距离（应该稳定在这里）")
    
    print(f"\n最近邻距离统计:")
    print(f"  最小值: {nearest_distances.min():.4f}")
    print(f"  最大值: {nearest_distances.max():.4f}")
    print(f"  平均值: {nearest_distances.mean():.4f}")
    print(f"  中位数: {np.median(nearest_distances):.4f}")
    print(f"  标准差: {nearest_distances.std():.4f}")
    
    # 检查问题
    too_close = nearest_distances < 0.5 * sigma  # 小于0.5σ可能有问题
    very_close = nearest_distances < r_eq * 0.9  # 小于90%平衡距离
    at_equilibrium = np.abs(nearest_distances - r_eq) < 0.1 * r_eq  # 在平衡距离±10%
    
    print(f"\n距离分布:")
    print(f"  < 0.5σ ({0.5*sigma:.2f}): {too_close.sum()} 粒子 ({100*too_close.sum()/len(positions):.2f}%)")
    print(f"  < 0.9×r_eq ({r_eq*0.9:.2f}): {very_close.sum()} 粒子 ({100*very_close.sum()/len(positions):.2f}%)")
    print(f"  在平衡距离附近 ({r_eq*0.9:.2f}-{r_eq*1.1:.2f}): {at_equilibrium.sum()} 粒子 ({100*at_equilibrium.sum()/len(positions):.2f}%)")
    
    if too_close.sum() > 0:
        print(f"\n⚠️ 警告: {too_close.sum()} 个粒子间距 < 0.5σ，可能有重叠或融合！")
        worst_idx = np.argmin(nearest_distances)
        print(f"   最近的一对: 距离 = {nearest_distances[worst_idx]:.4f}")
        print(f"   粒子位置: {positions[worst_idx]}")
    
    # 直方图
    bins = [0, 0.5*sigma, 0.8*sigma, r_eq*0.9, r_eq*1.1, 1.5*sigma, 2.0*sigma, 3.0*sigma, 10.0*sigma]
    hist, _ = np.histogram(nearest_distances, bins=bins)
    print(f"\n距离直方图:")
    for i in range(len(bins)-1):
        print(f"  [{bins[i]:.2f}, {bins[i+1]:.2f}): {hist[i]:5d} ({100*hist[i]/len(positions):5.2f}%)")
    
    return nearest_distances

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python check_distances.py <xyz_file>")
        sys.exit(1)
    
    filename = sys.argv[1]
    positions = read_xyz(filename)
    distances = analyze_distances(positions)
