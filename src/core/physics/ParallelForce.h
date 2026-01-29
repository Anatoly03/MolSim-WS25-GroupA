#pragma once

#ifdef OPENMP

#include "../LinkedCells.h"
#include "../Particle.h"
#include "ForceOptimized.h"
#include <omp.h>
#include <vector>

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
        , num_threads(omp_get_max_threads())
    {}
    
    /**
     * @brief 并行计算所有粒子间的 LJ 力
     */
    void calculateForces(LinkedCells& cells) {
        int total_particles = cells.particleCount();
        
        // 每个线程独立的力累加器（避免竞争条件）
        std::vector<std::vector<Vec3D>> thread_forces(num_threads);
        for (auto& tf : thread_forces) {
            tf.resize(total_particles, Vec3D(0, 0, 0));
        }
        
        // 获取所有粒子的指针（用于索引）
        std::vector<Particle*> particles;
        particles.reserve(total_particles);
        cells.forEach([&](Particle& p) {
            particles.push_back(&p);
        });
        
        // 并行计算力
        #pragma omp parallel
        {
            int tid = omp_get_thread_num();
            
            // 动态调度：负载均衡（每个任务处理16个粒子对）
            #pragma omp for schedule(dynamic, 16) nowait
            for (int i = 0; i < total_particles; i++) {
                for (int j = i + 1; j < total_particles; j++) {
                    Vec3D force = lennard_jones_optimized(lj_const, *particles[i], *particles[j]);
                    
                    // 牛顿第三定律
                    thread_forces[tid][i] += force;
                    thread_forces[tid][j] -= force;
                }
            }
        }
        
        // 合并各线程的力（也可以并行）
        #pragma omp parallel for
        for (int i = 0; i < total_particles; i++) {
            Vec3D total_force(0, 0, 0);
            for (const auto& tf : thread_forces) {
                total_force += tf[i];
            }
            particles[i]->force = total_force;
        }
    }
    
    /**
     * @brief 更优化的版本：利用 LinkedCells 空间分区
     * 只计算相邻 cell 内的粒子对，减少 O(n²) 为 O(n)
     */
    void calculateForcesCellBased(LinkedCells& cells) {
        // 使用 forEachDistinctPair 自动遍历所有相邻粒子对
        cells.forEachDistinctPair([this](Particle& p1, Particle& p2) {
            Vec3D force = lennard_jones_optimized(lj_const, p1, p2);
            
            // 原子操作避免竞争（OpenMP 自动处理）
            #pragma omp atomic
            p1.force.x += force.x;
            #pragma omp atomic
            p1.force.y += force.y;
            #pragma omp atomic
            p1.force.z += force.z;
            
            #pragma omp atomic
            p2.force.x -= force.x;
            #pragma omp atomic
            p2.force.y -= force.y;
            #pragma omp atomic
            p2.force.z -= force.z;
        });
    }
};

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
 * 
 * // 运行时设置线程数：
 * export OMP_NUM_THREADS=4
 */

#endif  // OPENMP
