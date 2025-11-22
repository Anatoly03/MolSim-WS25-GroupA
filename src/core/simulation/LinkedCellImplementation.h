
#pragma once

#include "../Args.h"
#include "Simulation.h"

class LinkedCellImplementation : public Simulation {
   private:
    ParticleContainer& particles;

   public:
    /**
     * @note Default constructor without providing particle container is private.
     */
    LinkedCellImplementation() = delete;
    // LinkedCellImplementation() : arguments(), particles(), writer(particles) {}

    /**
     * @brief Default constructor
     */
    LinkedCellImplementation(ParticleContainer &p, const Args &args) : Simulation(args), particles(p) {}

   public:
    /**
     * @brief Advance the simulation by one time step.
     */
    void tick() override {
        // TODO work here
    }
};
