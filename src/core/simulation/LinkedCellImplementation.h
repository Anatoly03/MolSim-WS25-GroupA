
#pragma once

#include "../utils/Args.h"
#include "../math/Vec3.h"
#include "../LinkedCells.h"
#include "../ParticleContainer.h"
#include "Simulation.h"

class LinkedCellImplementation : public Simulation {
   private:
    ParticleContainer& particles;
    std::vector<std::list<Particle>> cells;
    int cellSize;
    int cutOff;

    std::map<Vec3<int>, std::vector<Particle>> cells;

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
    LinkedCellImplementation(const Args &args, ParticleContainer &p,int size, int xOfDomain, int yOfDomain, int zOfDomain,int cutOff) : Simulation(args), particles(p) {

        nx = static_cast<int>(std::ceil(xOfDomain / static_cast<double>(size))) + 2;
        ny = static_cast<int>(std::ceil(yOfDomain / static_cast<double>(size))) + 2;
        nz = static_cast<int>(std::ceil(zOfDomain / static_cast<double>(size))) + 2;

        cells.resize(nx * ny * nz);
        cellSize=size;
        placeInCells(p);
        this->cutOff=cutOff;
    }

   private:
    /**
     * @brief Places all particles into correct cells.
     */
    void placeInCells(ParticleContainer &p);

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
     * @brief calculate the force for all particles
     */
    void calculateForce();

public:
    /**
     * @brief Advance the simulation by one time step.
     */
    void tick() override {
        // TODO work here
        //calculatePosition();
        //we should then first check the boundary cells and then the inner cells
        delayForce();
        calculateForce();
        calculateVelocity();

    }
};
