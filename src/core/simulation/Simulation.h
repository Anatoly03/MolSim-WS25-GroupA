
#pragma once

#include <functional>

#include "../math/Vec3.h"
#include "../utils/Args.h"
#include "../utils/ArrayUtils.h"
#include "../physics/Force.h"
#include "../ParticleContainer.h"
#include "../Particle.h"

#include "spdlog/spdlog.h"

class Simulation {
   protected:
    /**
     * @brief CLI input arguments, constant for the simulation run.
     */
    const Args arguments;

    /**
     * @brief Force calculation method
     */
    force_calculation_system forceCalculationSystem = lennard_jones_system;

    /**
     * @brief Current simulation iteration.
     */
    int iteration = 0;

   public:
    typedef const std::function<void(int/*iteration*/, Simulation&)> cb_type;

    /**
     * @note Default constructor without providing particle container is private.
     */
    Simulation() = delete;

    /**
     * @brief Default constructor
     */
    Simulation(const Args &args) : arguments(args) {
        // use the attraction provided by args
        forceCalculationSystem = get_force_system_by_name(args.attraction_method);
    }

    /**
     * @brief Destructor
     */
    virtual ~Simulation() = default;

   private:
    /**
     * @brief Plot the particles of a particular iteration to a file.
     */
    void plotParticles(const cb_type &callback) {
        if (arguments.benchmark_enabled) return;
        callback(iteration, *this);
    }

   public:
    // /**
    //  * @brief calculate the force for two distinct particles
    //  */
    // virtual double calculateLennardJonesPotential(Particle& p1, Particle& p2, double epsilon, double sigma);

    /**
     * @brief Updates the position for a single particles.
     */
    virtual void calculateSinglePosition(Particle &particle, double dt);

    /**
     * @brief Updates the velocity for a single particles.
     */
    virtual void calculateSingleVelocity(Particle &particle, double dt);

    /**
     * @brief Updates the position for a all particles.
     */
    virtual void calculatePosition() {
        forEachParticle([this](Particle &particle) {
            calculateSinglePosition(particle, arguments.delta_t);
        });
    }

    /**
     * @brief Updates the velocity for a all particles.
     */
    virtual void calculateVelocity() {
        forEachParticle([this](Particle &particle) {
            calculateSingleVelocity(particle, arguments.delta_t);
        });
    }

    /**
     * @brief calculate the force for all particles
     */
    virtual void calculateForce() {
        forEachDistinctParticlePair([&](Particle &p_i, Particle &p_j) {
            forceCalculationSystem(const_cast<Args&>(arguments), p_i, p_j);
        });
    }

    /**
     * @brief Delays the position for all particles.
     */
    virtual void delayPosition() {
        forEachParticle([this](Particle &particle) {
            particle.delayPosition();
        });
    }

    /**
     * @brief Delays the force for all particles.
     */
    virtual void delayForce() {
        forEachParticle([this](Particle &particle) {
            particle.delayForce();
        });
    }

   public:
    /**
     * @brief Iteration over every particle for writer callback.
     * @param callback Function to be called for each particle.
     * @example
     * ```c++
     * Simulation simulation;
     *
     * container.forEachParticle([](Particle &particle) {
     *     std::cout << particle.toString() << std::endl;
     * });
     * ```
     */
    virtual void forEachParticle(const std::function<void(Particle &)> &/*callback*/) {}
    
    /**
     * @brief Iteration over every particle for writer callback.
     * @param callback Function to be called for each particle.
     * @example
     * ```c++
     * Simulation simulation;
     *
     * container.forEachParticlePair([](Particle &particle, Particle &other) {
     *     std::cout << particle.toString() << " interacts with " << other.toString() << std::endl;
     * });
     * ```
     */
    virtual void forEachDistinctParticlePair(const std::function<void(Particle &, Particle &)> &/*callback*/) {}
    
    /**
     * @brief Total amount of tracked particles.
     */
    virtual int particleCount() {
        return 0;
    }

    /**
     * @brief Advance the simulation by one time step.
     * @note This performs one calculation step of the simulation.
     * No prints are performed and this method is benchmark viable.
     */
    virtual void tick() {}

    /**
     * @brief Run the simulation for a given time with specified time step.
     */
    virtual void run(const cb_type &callback) {
        const double start = arguments.start_time;
        const double end = arguments.end_time;
        const double delta_t = arguments.delta_t;

        plotParticles(callback);

        for (double t = start; t < end; t += delta_t) {
            tick();
            iteration++;

            if (iteration % arguments.output_interval == 0) {
                plotParticles(callback);
            }

            spdlog::debug("Iteration {} finished.", iteration);
        }

        spdlog::info("Output written. Terminating...");
    }

    //
    // STATIC
    //

    /**
     * @brief Factory method to create a simulation instance based on args.
     */
    static std::shared_ptr<Simulation> createSimulation(ParticleContainer &particles, const Args &args);
};
