
#pragma once

#include "../math/Vec3.h"
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
     * @brief Iteration over every distinct particle pair for writer callback.
     * @param callback Function to be called for each particle pair.
     * @example
     * ```c++
     * DirectSumAlgorithm simulation;
     *
     * simulation.forEachDistinctParticlePair([](Particle &particle, Particle &other) {
     *     std::cout << particle.toString() << " interacts with " << other.toString() << std::endl;
     * });
     * ```
     */
    void forEachDistinctParticlePair(const std::function<void(Particle &, Particle &)> &callback) override {
        particles.forEachDistinctPair(callback);
    }

    /**
     * @brief Total amount of tracked particles.
     */
    int particleCount() override {
        return particles.particleCount();
    }

    /**
     * @brief Advance the simulation by one time step.
     * @note This performs position, force, and velocity calculations.
     * No prints are performed and this method is benchmark viable.
     */
    void tick() override {
        calculatePosition(); // implemented in super class
        delayForce(); // implemented in super class
        calculateForce();
        applyGravity();
        calculateVelocity(); // implemented in super class
    }
};
