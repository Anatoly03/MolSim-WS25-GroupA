
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
    LinkedCellImplementation(ParticleContainer &particles, const Args &args, Vec3I cellSize) : Simulation(args), cells(cellSize) {

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
        cells.setDomainSize(Vec3I(-1), Vec3I(5)); // TODO pass domain size by arguments.
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
     * @brief calculate the position for all particles
     */
    void calculatePosition();

    /**
     * @brief calculate the velocity for all particles
     */
    void calculateVelocity();

    /**
     * @brief delay the force for all particles
     */
    void delayForce();

    /**
     * @brief calculate the force for two distinct particles
     */
    void calculateForce(Particle& p1, Particle& p2);

    /**
     * @brief calculate the force for all particles
     */
    void calculateForce() {
        cells.forEachDistinctPair([&](Particle &p_i, Particle &p_j) {
            calculateForce(p_i, p_j);
        });
    }

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

    // /**
    //  *  @brief returns the index of a particle
    //  */
    // Vec3<int> getIndex(Particle &p);

   public:
    /**
     * @brief Advance the simulation by one time step.
     * @note This performs position, force, and velocity calculations.
     * No prints are performed and this method is benchmark viable.
     */
    void tick() override {
        calculatePosition();
        delayForce();
        calculateForce();
        // calculateBorderBehaviour();
        calculateVelocity();
        reindexParticles();
    }
};
