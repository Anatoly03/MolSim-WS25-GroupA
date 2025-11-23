
#pragma once

#include "../utils/Args.h"
#include "../math/Vec3.h"
#include "../LinkedCells.h"
#include "../ParticleContainer.h"
#include "Simulation.h"

class LinkedCellImplementation : public Simulation {
   private:
    ParticleContainer& particles;
    //std::vector<std::list<Particle>> cells;
    //int cellSize;
    int cutOff;

    std::map<Vec3<int>, std::vector<Particle*>> cells;

    Vec3D cellSize = Vec3D(1.0);

   public:
    int nx=0;
    int ny=0;
    int nz=0;
    /**
     * @note Default constructor without providing particle container is private.
     */
    LinkedCellImplementation() = delete;
    // DirectSumAlgorithm() : arguments(), particles(), writer(particles) {}

    /**
     * @brief the +2 contribute to creating ghost cells which helps with boundery
     */
    LinkedCellImplementation( ParticleContainer &p, const Args &args,int size, int xOfDomain, int yOfDomain, int zOfDomain,int cutOff) : Simulation(args), particles(p) {

        nx = static_cast<int>(std::ceil(xOfDomain / static_cast<double>(size))) + 2;
        ny = static_cast<int>(std::ceil(yOfDomain / static_cast<double>(size))) + 2;
        nz = static_cast<int>(std::ceil(zOfDomain / static_cast<double>(size))) + 2;

        //cells.resize(nx * ny * nz);
        //cellSize=size;
        placeInCells();
        this->cutOff=cutOff;
    }

   private:
    /**
     * @brief Places all particles into correct cells.
     */
    void placeInCells();

    /**
    * @brief return all boundary particles .
    */
    std::vector<Particle*> boundaryParticles();


    /**
    * @brief return all inner particles .
    */
    std::vector<Particle*> innerParticles();

    /**
      * @brief calculate the position for all particles
      */
    void calculatePosition(std::vector<Particle*> &p);

    /**
     * @brief calculate the velocity for all particles
     */
    void calculateVelocity();

    /**
     * @brief delay the force for all particles
     */
    void delayForce();

    /**
     * @brief calculate the force for all particles
     */
    void calculateForce();

    /**
     * @brief return all particles in ghost/halo cells
     */
    std::vector<Particle*> ghostCellParticles();

    /**
     * @brief for each methode for all boundary particles
     */
    void forEachBoundaryParticles(const std::function<void(Particle &)> &callback);

    /**
     * @brief for each methode for all inner particles
     */
    void forEachInnerParticles(const std::function<void(Particle &)> &callback);

    /**
     * @brief for each methode for all ghost/halo particles
     */
    void forEachGhostParticles(const std::function<void(Particle &)> &callback);

    /**
     * @brief for each methode for all ghost/halo particles
     */
    void forEachDistinctPair(const std::function<void(Particle &, Particle&)> &callback);

    /**
     * @brief deletes ghost cell particles
     */
    void deleteGhostCellParticles();

public:
    /**
     * @brief Advance the simulation by one time step.
     */
    void tick() override {
        // TODO work here
        //calculatePosition();
        //we should then first check the boundary cells and then the inner cells
        std::vector<Particle*> particlesVec=boundaryParticles();
        calculatePosition(particlesVec);
        particlesVec=innerParticles();
        calculatePosition(particlesVec);


        delayForce();
        calculateForce();
        calculateVelocity();

    }
};
