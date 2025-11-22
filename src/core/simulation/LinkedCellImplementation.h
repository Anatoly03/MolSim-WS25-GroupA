
#pragma once

#include "../Args.h"
#include "Simulation.h"

class DirectSumAlgorithm : public Simulation {
   private:
    ParticleContainer& particles;

   public:
    /**
     * @note Default constructor without providing particle container is private.
     */
    DirectSumAlgorithm() = delete;
    // DirectSumAlgorithm() : arguments(), particles(), writer(particles) {}

    /**
     * @brief Default constructor
     */
    DirectSumAlgorithm(ParticleContainer &p, const Args &args) : Simulation(args), particles(p) {}

   public:
    /**
     * @brief Advance the simulation by one time step.
     */
    void tick() override {
        // TODO work here
    }
};
