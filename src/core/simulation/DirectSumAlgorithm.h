
#pragma once

#include "../Args.h"
#include "../ParticleContainer.h"
#include "../utils/ArrayUtils.h"
#include "../writer/Writer.h"
#include "Simulation.h"

#include "spdlog/spdlog.h"

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

   private:
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
     * @note This performs position, force, and velocity calculations.
     * No prints are performed and this method is benchmark viable.
     */
    void tick() override {
        calculatePosition();
        delayForce();
        calculateForce();
        calculateVelocity();
    }
};
