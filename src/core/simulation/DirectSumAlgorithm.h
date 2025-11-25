
#pragma once

#include "../utils/Args.h"
#include "../ParticleContainer.h"
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
    void calculateForce() {
        particles.forEachDistinctPair([&](Particle &p_i, Particle &p_j) {
            calculateSingleForce(p_i, p_j);
        });
    }

   public:
    /**
     * @brief Iteration over every particle for writer callback.
     * @param callback Function to be called for each particle.
     * @example
     * ```c++
     * DirectSumAlgorithm simulation;
     *
     * simulation.forEachParticle([](Particle &particle) {{
     *     std::cout << particle.toString() << std::endl;
     * });
     * ```
     */
    void forEachParticle(const std::function<void(Particle &)> &callback) override {
        particles.forEach(callback);
    }

    /**
     * @brief Total amount of tracked particles.
     */
    int particleCount() override {
        return particles.size();
    }

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
