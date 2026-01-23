#pragma once

#include "../LinkedCells.h"
#include "../Particle.h"
#include "ForceOptimized.h"
#ifdef OPENMP
# include <omp.h>
#endif
#include <vector>
#include <algorithm>

/**
 * @brief OpenMP 并行化的力计算
 * 性能提升：2-4倍 (在4核CPU上)
 * 
 * 编译要求：
 * cmake -DCMAKE_CXX_FLAGS="-O3 -fopenmp -march=native"
 */
class ParallelForceCalculator {
private:
    LJConstants lj_const;
    int num_threads;
    
public:
    explicit ParallelForceCalculator(const Args& args) 
        : lj_const(args)
        , num_threads(1)
    {}
    
    /**
     * @brief 并行计算所有粒子间的 LJ 力
     */
    void calculateForces(LinkedCells& cells) {
        int total_particles = cells.particleCount();
        // 获取所有粒子的指针（用于索引），然后并行遍历索引
        std::vector<Particle*> particles;
        particles.reserve(total_particles);
        cells.forEach([&](Particle& p) { particles.push_back(&p); });

#ifdef OPENMP
        int runtime_threads = omp_get_max_threads();
#else
        int runtime_threads = 1;
#endif

        // 每个线程独立的力累加器（避免竞争条件），在确定线程数后分配
        std::vector<std::vector<Vec3D>> thread_forces(runtime_threads, std::vector<Vec3D>(total_particles, Vec3D(0,0,0)));

        // 并行计算力：对 i 进行并行划分，内部遍历 j>i
#ifdef OPENMP
        #pragma omp parallel for schedule(dynamic, 16)
#endif
        for (int i = 0; i < total_particles; i++) {
#ifdef OPENMP
            int tid = omp_get_thread_num();
#else
            int tid = 0;
#endif
            for (int j = i + 1; j < total_particles; j++) {
                Vec3D force = lennard_jones_optimized(lj_const, *particles[i], *particles[j]);
                thread_forces[tid][i] += force;
                thread_forces[tid][j] -= force;
            }
        }

        // 合并各线程的力（并行化合并）
#ifdef OPENMP
        #pragma omp parallel for
#endif
        for (int i = 0; i < total_particles; i++) {
            Vec3D total_force(0, 0, 0);
            for (const auto& tf : thread_forces) total_force += tf[i];
            particles[i]->force = total_force;
        }
    }
    
    /**
     * @brief 更优化的版本：利用 LinkedCells 空间分区
     * 只计算相邻 cell 内的粒子对，减少 O(n²) 为 O(n)
     */
    void calculateForcesCellBased(LinkedCells& cells) {
        // 清空所有粒子的力：先收集指针，再并行遍历索引
        std::vector<Particle*> particles;
        particles.reserve(cells.particleCount());
        cells.forEach([&](Particle& p){ particles.push_back(&p); });
#ifdef OPENMP
        #pragma omp parallel for
#endif
        for (int i = 0; i < (int)particles.size(); ++i) particles[i]->force = Vec3D(0,0,0);

        // Use the indexed distinct-pair iterator which can be parallelized inside LinkedCells.
        // The callback receives global particle indices; map them to Particle& and update forces atomically.
        cells.forEachDistinctPairIndexed([&](int idx1, int idx2) {
            Particle &p1 = cells.getParticleByIndex(idx1);
            Particle &p2 = cells.getParticleByIndex(idx2);
            Vec3D force = lennard_jones_optimized(lj_const, p1, p2);

            // Atomic updates per component
            #ifdef OPENMP
            #pragma omp atomic
            #endif
            p1.force.x += force.x;
            #ifdef OPENMP
            #pragma omp atomic
            #endif
            p1.force.y += force.y;
            #ifdef OPENMP
            #pragma omp atomic
            #endif
            p1.force.z += force.z;

            #ifdef OPENMP
            #pragma omp atomic
            #endif
            p2.force.x -= force.x;
            #ifdef OPENMP
            #pragma omp atomic
            #endif
            p2.force.y -= force.y;
            #ifdef OPENMP
            #pragma omp atomic
            #endif
            p2.force.z -= force.z;
        });
    }
};

/**
 * @brief 并行化的边界处理
 */
inline void applyBoundariesParallel(LinkedCells& cells) {
    const Vec3D domainMin = Vec3D(cells.domainMin);
    const Vec3D domainMax = Vec3D(cells.domainMax);

    // 预计算边界标志：输入文件中通常使用整数（0 delete | 1 reflective | 2 periodic）
    bool reflectXmin = (cells.boarderXmin == 1);
    bool reflectXmax = (cells.boarderXmax == 1);
    bool reflectYmin = (cells.boarderYmin == 1);
    bool reflectYmax = (cells.boarderYmax == 1);
    bool reflectZmin = (cells.boarderZmin == 1);
    bool reflectZmax = (cells.boarderZmax == 1);

    // 收集粒子指针并并行遍历索引
    std::vector<Particle*> particles;
    particles.reserve(cells.particleCount());
    cells.forEach([&](Particle &p){ particles.push_back(&p); });

#ifdef OPENMP
    #pragma omp parallel for
#endif
    for (int i = 0; i < (int)particles.size(); ++i) {
        Particle &p = *particles[i];
        // X 边界
        if (reflectXmin && p.position.x < domainMin.x) {
            p.position.x = domainMin.x;
            p.velocity.x = std::abs(p.velocity.x);
        } else if (reflectXmax && p.position.x > domainMax.x) {
            p.position.x = domainMax.x;
            p.velocity.x = -std::abs(p.velocity.x);
        }

        // Y 边界
        if (reflectYmin && p.position.y < domainMin.y) {
            p.position.y = domainMin.y;
            p.velocity.y = std::abs(p.velocity.y);
        } else if (reflectYmax && p.position.y > domainMax.y) {
            p.position.y = domainMax.y;
            p.velocity.y = -std::abs(p.velocity.y);
        }

        // Z 边界
        if (reflectZmin && p.position.z < domainMin.z) {
            p.position.z = domainMin.z;
            p.velocity.z = std::abs(p.velocity.z);
        } else if (reflectZmax && p.position.z > domainMax.z) {
            p.position.z = domainMax.z;
            p.velocity.z = -std::abs(p.velocity.z);
        }
    }
}

/**
 * @brief 使用示例
 * 
 * // 在 CMakeLists.txt 添加：
 * find_package(OpenMP REQUIRED)
 * target_link_libraries(MolSim PRIVATE OpenMP::OpenMP_CXX)
 * 
 * // 在代码中使用：
 * ParallelForceCalculator force_calc(args);
 * 
 * // 每次迭代：
 * force_calc.calculateForcesCellBased(cells);
 * applyBoundariesParallel(cells);
 * 
 * // 运行时设置线程数：
 * export OMP_NUM_THREADS=4
 */
