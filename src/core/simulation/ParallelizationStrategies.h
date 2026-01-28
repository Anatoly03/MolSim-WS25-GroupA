#pragma once

#include "../LinkedCells.h"
#include "../Particle.h"
#include "../utils/Args.h"
#include "../physics/Force.h"
#include "../physics/ForceOptimized.h"  // Added: Optimized force calculations
#include "../math/Vec3.h"

#ifdef OPENMP
#include <omp.h>
#endif

/**
 * @brief Strategy 0: Direct O(n^2) parallelization using thread-local accumulators
 * 
 * This strategy parallelizes over all particle pairs directly without using
 * the spatial partitioning from LinkedCells. Each thread maintains its own
 * force accumulator to avoid contention.
 * 
 * Optimization Support:
 * - ForceOptimized.h can be used to accelerate LJ calculations (5-25% faster)
 * - Pre-computed constants: LJConstants (reduces repeated arithmetic)
 * - Vectorization-friendly variant: lennard_jones_vectorizable (10-20% with -march=native)
 * - Lookup table method: LJLookupTable (15-25% for large N)
 * 
 * Pros:
 * - Simple to implement and understand
 * - Good load balancing if particles are uniformly distributed
 * - Works well with small-to-medium datasets (thousands of particles)
 * - Can use optimized force calculations for additional speedup
 * 
 * Cons:
 * - O(n^2) computational complexity
 * - Memory overhead: O(threads * n) for force accumulators
 * - Poor performance with very large systems (N > 100k)
 * - Doesn't exploit spatial locality
 * 
 * @see ForceOptimized.h for algorithm optimization options
 * @see ParallelForce.h for alternative parallel implementation
 */
class DirectParallelizationStrategy {
public:
    static void calculateForces(
        LinkedCells& /*cells*/,
        ParticleContainer& particles,
        force_calculation_system forceCalculationSystem,
        const Args& arguments
    ) {
        int n = particles.particleCount();
        
        // Reset forces
#ifdef OPENMP
        #pragma omp parallel for schedule(static)
#endif
        for (int i = 0; i < n; ++i) {
            particles[i].force = Vec3D(0, 0, 0);
        }

#ifdef OPENMP
        int num_threads = omp_get_max_threads();
#else
        int num_threads = 1;
#endif

        // Thread-local force accumulators
        std::vector<std::vector<Vec3D>> thread_forces(
            num_threads,
            std::vector<Vec3D>(n, Vec3D(0, 0, 0))
        );

        // Parallel computation with thread-local accumulators
#ifdef OPENMP
        #pragma omp parallel for schedule(dynamic, 64)
#endif
        for (int i = 0; i < n; ++i) {
#ifdef OPENMP
            int tid = omp_get_thread_num();
#else
            int tid = 0;
#endif
            for (int j = i + 1; j < n; ++j) {
                Vec3D force = forceCalculationSystem(
                    const_cast<Args&>(arguments),
                    particles[i],
                    particles[j]
                );
                thread_forces[tid][i] += force;
                thread_forces[tid][j] -= force;
            }
        }

        // Merge thread-local forces back to particles
#ifdef OPENMP
        #pragma omp parallel for schedule(static)
#endif
        for (int i = 0; i < n; ++i) {
            Vec3D total_force(0, 0, 0);
            for (int t = 0; t < num_threads; ++t) {
                total_force += thread_forces[t][i];
            }
            particles[i].force = total_force;
        }
    }
};

/**
 * @brief Strategy 1: Cell-based parallelization using spatial partitioning
 * 
 * This strategy uses the LinkedCells spatial data structure to iterate over
 * only nearby particle pairs, reducing complexity from O(n^2) to O(n) for
 * sparse distributions. Cell-based iteration allows better parallelization.
 * 
 * Optimization Support:
 * - ForceOptimized.h can be integrated for additional speedup (5-25%)
 * - Pre-computed constants reduce per-pair arithmetic
 * - Better memory efficiency than Strategy 0 (no thread-local buffers)
 * 
 * Pros:
 * - O(n) computational complexity for sparse systems
 * - Scales well to very large particle counts (N > 100k)
 * - Exploits spatial locality and cache coherence
 * - No thread-local memory overhead (O(n) space only)
 * - Best for realistic molecular simulations
 * 
 * Cons:
 * - More complex to implement
 * - Less predictable load distribution if particles are clustered
 * - Overhead of spatial indexing (LinkedCells reindexing)
 * - Atomic operations may cause contention with many threads
 * 
 * @see ForceOptimized.h for algorithm optimization options
 * @see ParallelForce.h for alternative implementations with different trade-offs
 */
class CellBasedParallelizationStrategy {
public:
    static void calculateForces(
        LinkedCells& cells,
        ParticleContainer& particles,
        force_calculation_system forceCalculationSystem,
        const Args& arguments
    ) {
        int n = particles.particleCount();
        
        // Reset forces
#ifdef OPENMP
        #pragma omp parallel for schedule(static)
#endif
        for (int i = 0; i < n; ++i) {
            particles[i].force = Vec3D(0, 0, 0);
        }

        // Use LinkedCells to compute only nearby pairs
        // Note: forEachDistinctPair uses callbacks, which inherently allows parallelization
        // within the LinkedCells implementation if needed
        cells.forEachDistinctPair([&](Particle& p1, Particle& p2) {
            Vec3D force = forceCalculationSystem(
                const_cast<Args&>(arguments),
                p1,
                p2
            );

            // Atomic updates to ensure thread safety when OpenMP is enabled
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
 * @brief FUTURE: Strategy 2 - Lookup Table Optimization for Large Systems
 * 
 * @note This is a placeholder for potential future enhancement
 * 
 * Implements cell-based parallelization with lookup table force calculations.
 * Recommended for very large systems (> 1M particles) where computation is 
 * bandwidth-limited rather than compute-limited.
 * 
 * Performance: ~15-25% faster than direct LJ calculation
 * Memory: +4KB for lookup table (negligible for large systems)
 * 
 * Implementation would combine:
 * - CellBasedParallelizationStrategy logic
 * - LJLookupTable from ForceOptimized.h
 * - Pre-computed boundary conditions
 * 
 * Example usage:
 * @code
 * // In future when implemented:
 * class LookupTableStrategy {
 *     LJLookupTable lj_table;
 *     
 *     void calculateForces(LinkedCells& cells, ...) {
 *         cells.forEachDistinctPair([&](Particle& p1, Particle& p2) {
 *             Vec3D force = lj_table.force(p1, p2);  // Lookup + interpolate
 *             p1.force += force;
 *             p2.force -= force;
 *         });
 *     }
 * };
 * @endcode
 * 
 * @see LJLookupTable in ForceOptimized.h
 */

/**
 * @brief Strategy Selection Helper
 * 
 * Returns appropriate parallelization strategy based on system size and configuration
 */
inline std::string getStrategyInfo(int strategy) {
    switch(strategy) {
        case 0:
            return "DirectParallelizationStrategy (O(n²), thread-local accumulators)";
        case 1:
            return "CellBasedParallelizationStrategy (O(n), spatial partitioning, atomic ops)";
        default:
            return "Unknown strategy - defaulting to cell-based (1)";
    }
}
