
#pragma once

#include "../utils/Args.h"
#include "../utils/TracyHelper.h"
#include "../math/Vec3.h"
#include "../LinkedCells.h"
#include "../ParticleContainer.h"
#include "Simulation.h"

#include "spdlog/spdlog.h"
#include <fmt/format.h>

#ifdef OPENMP
#include <omp.h>
#endif

class LinkedCellImplementation : public Simulation {
   public:
    LinkedCells cells;

   public:
    Vec3I domainMin = Vec3I(1.0);
    Vec3I domainMax = Vec3I(1.0);

    // int nx=0;
    // int ny=0;
    // int nz=0;

    /**
     * @note Default constructor without providing particle container is private.
     */
    LinkedCellImplementation() = delete;

    /**
     * @brief the +2 contribute to creating ghost cells which helps with boundery
     */
    LinkedCellImplementation(ParticleContainer &particles, const Args &args) :
        Simulation(particles, args),
        cells(std::function<Particle&(int)>([this](int index) -> Particle& { return _internal_particle_getter(index); }), args.cell_size)
    {
        // constants, to be set later
        // double size = 10;
        // double xOfDomain = 40;
        // double yOfDomain = 40;
        // double zOfDomain = 40;
        // double cutOff = 2.5;

        // nx = static_cast<int>(std::ceil(xOfDomain / static_cast<double>(size))) + 2;
        // ny = static_cast<int>(std::ceil(yOfDomain / static_cast<double>(size))) + 2;
        // nz = static_cast<int>(std::ceil(zOfDomain / static_cast<double>(size))) + 2;

        // //cells.resize(nx * ny * nz);
        // //cellSize=size;
        // setMinMax();

        cells.absorb(particles);
        cells.setDomainSize(args.domain_min, args.domain_max);
        domainMin=args.domain_min;
        domainMax=args.domain_max;
        cells.setBorder(args.boarderXmin,args.boarderXmax,args.boarderYmin,args.boarderYmax,args.boarderZmin,args.boarderZmax);
        auto removedCells = cells.clearOutOfBoundsCells();
        if (removedCells != 0) {
            spdlog::warn("out of bounds particles in {} cells removed", removedCells);
        }

        // this->cutOff=cutOff;
    }

   private:
    Particle& _internal_particle_getter(int index) {
        return const_cast<Particle&>(particles[index]);
    }

    // /**
    //  * @brief Places all particles into correct cells.
    //  */
    // void placeInCells(ParticleContainer &p);

    // /**
    // * @brief return all boundary particles .
    // */
    // std::vector<Particle*> boundaryParticles();

    // /**
    // * @brief return all inner particles .
    // */
    // std::vector<Particle*> innerParticles();

    /**
     * @brief reindex particles into new chunks
     */
    void reindexParticles();

    /**
     * @brief calculate special physics for domain-bordering cells, e.g.
     * particle-border collision or particle extinction
     */
    void calculateBorderBehaviour();

    // /**
    //  * @brief return all particles in ghost/halo cells
    //  */
    // std::vector<Particle*> ghostCellParticles();

    // /**
    //  * @brief for each methode for all boundary particles
    //  */
    // void forEachBoundaryParticles(const std::function<void(Particle &)> &callback);

    // /**
    //  * @brief for each methode for all inner particles
    //  */
    // void forEachInnerParticles(const std::function<void(Particle &)> &callback);

    // /**
    //  * @brief for each methode for all ghost/halo particles
    //  */
    // void forEachGhostParticles(const std::function<void(Particle &)> &callback);

    // /**
    //  * @brief for each methode for all ghost/halo particles
    //  */
    // void forEachDistinctPair(const std::function<void(Particle &, Particle&)> &callback);

    // /**
    //  * @brief deletes ghost cell particles
    //  */
    // void deleteGhostCellParticles();

    // /**
    //  *  @brief gets the offset (due to the inclusion of ghost/halo cells) for index calculation
    //  */
    // void setMinMax();

   public:
    /**
     * @brief Iteration over every particle for writer callback.
     * @param callback Function to be called for each particle.
     * @example
     * ```c++
     * LinkedCellImplementation simulation;
     *
     * simulation.forEachParticle([](Particle &particle) {{
     *     std::cout << particle.toString() << std::endl;
     * });
     * ```
     */
    void forEachParticle(const std::function<void(Particle &)> &callback) override {
        cells.forEach(callback);
    }

    /**
     * @brief Iteration over every distinct particle pair for writer callback.
     * @param callback Function to be called for each particle pair.
     * @example
     * ```c++
     * LinkedCellImplementation simulation;
     *
     * simulation.forEachDistinctParticlePair([](Particle &particle, Particle &other) {
     *     std::cout << particle.toString() << " interacts with " << other.toString() << std::endl;
     * });
     * ```
     */
    void forEachDistinctParticlePair(const std::function<void(Particle &, Particle &)> &callback) override {
        cells.forEachDistinctPair(callback);
    }

    /**
     * @brief calculate the force for all particles (overridden to support parallel strategies)
     */
    void calculateForce() override {
        PROFILE_ZONE_NAMED("Force Calculation");

        // reset forces
        cells.forEach([&](Particle &p){ p.force = Vec3D(0); });

        using PS = Args::ParallelStrategy;

        if (arguments.parallel_strategy == PS::None) {
            cells.forEachDistinctPair([&](Particle &par1, Particle &par2) {
                Vec3D force = forceCalculationSystem(const_cast<Args &>(arguments), par1, par2);
                par1.force += force;
                par2.force -= force;
#ifdef TRACY_ENABLE
                forceParticlePairsPerSecond++;
#endif
            });
            return;
        }

        // Buffer strategy: per-thread accumulation buffers
        if (arguments.parallel_strategy == PS::Buffer) {
            const int nParticles = particles.particleCount();

#ifdef OPENMP
            // set runtime schedule from args
            omp_sched_t sched = omp_sched_static;
            if (arguments.omp_schedule == "dynamic") sched = omp_sched_dynamic;
            else if (arguments.omp_schedule == "guided") sched = omp_sched_guided;
            omp_set_schedule(sched, arguments.omp_chunk);
            const int nThreads = omp_get_max_threads();
#else
            const int nThreads = 1;
#endif

            std::vector<std::vector<Vec3D>> buffers(nThreads, std::vector<Vec3D>(nParticles, Vec3D(0)));

            cells.forEachDistinctPairIndexed([&](int i1, int i2) {
#ifdef OPENMP
                int tid = omp_get_thread_num();
#else
                int tid = 0;
#endif
                Vec3D f = forceCalculationSystem(const_cast<Args &>(arguments), particles[i1], particles[i2]);
                buffers[tid][i1] += f;
                buffers[tid][i2] -= f;
#ifdef TRACY_ENABLE
                forceParticlePairsPerSecond++;
#endif
            });

            // merge buffers
            for (int t = 0; t < nThreads; t++) {
                for (int i = 0; i < nParticles; i++) {
                    if (buffers[t][i].length() != 0.0) { // avoid some work
                        particles[i].force += buffers[t][i];
                    }
                }
            }

            return;
        }

        // Critical strategy: parallel iteration, protected updates
        if (arguments.parallel_strategy == PS::Critical) {
#ifdef OPENMP
            omp_sched_t sched = omp_sched_static;
            if (arguments.omp_schedule == "dynamic") sched = omp_sched_dynamic;
            else if (arguments.omp_schedule == "guided") sched = omp_sched_guided;
            omp_set_schedule(sched, arguments.omp_chunk);
#endif

            cells.forEachDistinctPairIndexed([&](int i1, int i2) {
                Vec3D f = forceCalculationSystem(const_cast<Args &>(arguments), particles[i1], particles[i2]);
#ifdef OPENMP
                #pragma omp critical
                {
                    particles[i1].force += f;
                    particles[i2].force -= f;
                }
#else
                particles[i1].force += f;
                particles[i2].force -= f;
#endif
#ifdef TRACY_ENABLE
                forceParticlePairsPerSecond++;
#endif
            });

            return;
        }

    }

    /**
     * @brief Total amount of tracked particles.
     */
    int particleCount() override {
        return cells.particleCount();
    }

    /**
     * @brief Advance the simulation by one time step.
     * @note This performs position, force, and velocity calculations.
     * No prints are performed and this method is benchmark viable.
     */
    void tick() override {
        calculatePosition(); // implemented in super class

        //clamping particles back into the domain.
        cells.forEach([&](Particle &p) {
            if (p.position.y < domainMin.y && arguments.boarderYmin == 1) {


                double penetration = domainMin.y - p.position.y;
                p.position.y = domainMin.y + penetration;
                Particle wall;
                //idea in testing: improving velocity smoothness with low sigma value for walls.
                wall.sigma = 0.01;
                wall.epsilon = 0.01;
                wall.position.y = domainMin.y - (wall.sigma * pow(2,1/6));
                wall.position.x = p.position.x;
                wall.position.z = p.position.z;


                auto f = forceCalculationSystem(const_cast<Args &>(arguments), p, wall);
                p.force += f;
                //p.velocity.y = -p.velocity.y;
            }
            if (p.position.y > domainMax.y && arguments.boarderYmax == 1) {
                double penetration = p.position.y - domainMax.y;
                p.position.y = domainMax.y - penetration;
                p.velocity.y = -p.velocity.y;
            }
            if (p.position.x < domainMin.x && arguments.boarderXmin == 1) {
                double penetration = domainMin.x - p.position.x;
                p.position.x = domainMin.x + penetration;
                p.velocity.x = -p.velocity.x;
            }
            if (p.position.x > domainMax.x && arguments.boarderXmax == 1) {
                double penetration = p.position.x - domainMax.x;
                p.position.x = domainMax.x - penetration;
                p.velocity.x = -p.velocity.x;
            }
            if (p.position.z < domainMin.z && arguments.boarderZmin == 1) {
            double penetration = domainMin.z - p.position.z;
            p.position.z = domainMin.z + penetration;
            p.velocity.z = -p.velocity.z;
        }
            if (p.position.z > domainMax.z && arguments.boarderZmax == 1) {
                double penetration = p.position.z - domainMax.z;
                p.position.z = domainMax.z - penetration;
                p.velocity.z = -p.velocity.z;
            }
        });
        delayPosition(); // implemented in super class
        reindexParticles();

        delayForce(); // implemented in super class
        calculateForce();
        applyGravity();
        calculateBorderBehaviour();
        calculateVelocity(); // implemented in super class

        PROFILE_PLOT("Active Cells", cells.cellCount());
    }
};
