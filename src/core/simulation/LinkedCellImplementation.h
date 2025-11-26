
#pragma once

#include "../utils/Args.h"
#include "../math/Vec3.h"
#include "../LinkedCells.h"
#include "../ParticleContainer.h"
#include "Simulation.h"

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
    LinkedCellImplementation(ParticleContainer &particles, const Args &args) : Simulation(args), cells(args.cell_size) {
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
        
        auto removedCells = cells.clearOutOfBoundsCells();
        if (removedCells != 0) {
            spdlog::warn("out of bounds particles in {} cells removed", removedCells);
        }

        // this->cutOff=cutOff;
    }

   private:
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
        reindexParticles();
        delayForce(); // implemented in super class
        calculateForce();
        // calculateBorderBehaviour();
        calculateVelocity(); // implemented in super class
    }
};
