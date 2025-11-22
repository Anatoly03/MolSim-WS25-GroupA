
#pragma once

#include "../utils/Args.h"
#include "Simulation.h"

class LinkedCellImplementation : public Simulation {
   private:
    ParticleContainer& particles;
    std::vector<std::list<Particle>> cells;
    int cellSize;

   public:
    /**
     * @note Default constructor without providing particle container is private.
     */
    LinkedCellImplementation() = delete;
    // DirectSumAlgorithm() : arguments(), particles(), writer(particles) {}

    /**
     * @brief Default constructor
     */
    LinkedCellImplementation(ParticleContainer &p, const Args &args) : Simulation(args), particles(p) {
        // TODO work here
        int cuttOff = 0;
        int xOfDomain = 0;
        int yOfDomain = 0;
        int zOfDomain = 0;

        cells.resize((xOfDomain/cuttOff) * (yOfDomain/cuttOff) * (zOfDomain/cuttOff));
        cellSize=cuttOff;
        placeInCells();
    }

   private:
    /**
     * @brief places all particles into correct cells.
     */
    void placeInCells();

   public:
    /**
     * @brief Advance the simulation by one time step.
     */
    void tick() override {
        // TODO work here
    }
};
