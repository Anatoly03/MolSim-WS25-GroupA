
#pragma once

#include "../utils/Args.h"
#include "../math/Vec3.h"
#include "../LinkedCells.h"
#include "../ParticleContainer.h"
#include "Simulation.h"

class LinkedCellImplementation : public Simulation {
   private:
    std::map<Vec3<int>, std::vector<Particle>> cells;
    
    Vec3D cellSize = Vec3D(1.0);

   public:
    /**
     * @note Default constructor without providing particle container is private.
     */
    LinkedCellImplementation() = delete;

    /**
     * @brief Default constructor
     */
    LinkedCellImplementation(ParticleContainer &p, const Args &args) : Simulation(args), cells() {
        // TODO work here

        int cuttOff = 0;
        int xOfDomain = 0;
        int yOfDomain = 0;
        int zOfDomain = 0;

        // cells.resize((xOfDomain/cuttOff) * (yOfDomain/cuttOff) * (zOfDomain/cuttOff));
        // cellSize=cuttOff;

        // Flood the cells with particles from the container.
        placeInCells(p);
    }

   private:
    /**
     * @brief Places all particles into correct cells.
     */
    void placeInCells(ParticleContainer &p);

   public:
    /**
     * @brief Advance the simulation by one time step.
     */
    void tick() override {
        // TODO work here
    }
};
