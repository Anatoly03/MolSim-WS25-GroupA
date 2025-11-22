
#pragma once

#include "../Args.h"
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
    LinkedCellImplementation(const Args &args, ParticleContainer &p,int cuttOff, int xOfDomain, int yOfDomain, int zOfDomain) : Simulation(args), particles(p){
        cells.resize((xOfDomain/cuttOff) * (yOfDomain/cuttOff) * (zOfDomain/cuttOff));
        cellSize=cuttOff;
        placeInCells();

    }

   public:
    /**
     * @brief Advance the simulation by one time step.
     */
    void tick() override {
        // TODO work here
    }

    /**
     * @brief places all particles into correct cells.
     */
    void placeInCells(){

        particles.forEach([&](Particle &p) {
            int cx = static_cast<int>(p.getPosition().x / cellSize);
            int cy = static_cast<int>(p.getPosition().y / cellSize);
            int cz = static_cast<int>(p.getPosition().z / cellSize);

            int index = cz * (p.getPosition().x * p.getPosition().y) + cy * p.getPosition().x + cx;
            cells[index].push_back(p);
        });



    }
};
