
#pragma once

#include <functional>
#include <chrono>

#include "../math/Vec3.h"
#include "../utils/Args.h"
#include "../utils/ArrayUtils.h"
#include "../utils/TracyHelper.h"
#include "../physics/Force.h"
#include "../ParticleContainer.h"
#include "../Particle.h"

#include "spdlog/spdlog.h"

#ifdef OPENMP
#include <omp.h>
#endif

class Simulation {
   protected:
   /**
     * @brief Reference to the particle container.
     */
    ParticleContainer& particles;


    /**
     * @brief CLI input arguments, constant for the simulation run.
     */
    const Args arguments;

    /**
     * @brief Force calculation method
     */
    force_calculation_system forceCalculationSystem = lennard_jones_system;

    /**
     * @brief Flag to track if force calculation is enabled
     */
    bool forceCalculationEnabled = true;

#ifdef TRACY_ENABLE
    /**
     * @brief Buffer counting amount of particle updates in a second interval.
     */
    int particleUpdatesPerSecond = 0;

    /**
     * @brief Buffer counting amount of particle updates in a second interval.
     */
    int forceParticlePairsPerSecond = 0;
#endif

    /**
     * @brief 
     */
    std::chrono::steady_clock::time_point lastSecondUpdate = std::chrono::steady_clock::now();

   public:
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
    Simulation(ParticleContainer &p, const Args &args) : arguments(args), particles(p) {
        // use the attraction provided by args
        forceCalculationSystem = get_force_system_by_name(args.attraction_method);
        forceCalculationEnabled = (args.attraction_method != "null");
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
#ifdef TRACY_ENABLE
        (void) callback;
#else
        if (arguments.benchmark_enabled) return;
        callback(iteration, *this);
#endif
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
        PROFILE_ZONE_NAMED("Position Calculation");

#ifdef OPENMP
        #pragma omp parallel for schedule(static)
#endif
        for (int i = 0; i < particles.particleCount(); ++i) {
            calculateSinglePosition(particles[i], arguments.delta_t);
        }
    }

    /**
     * @brief Updates the velocity for a all particles.
     */
    virtual void calculateVelocity() {
        PROFILE_ZONE_NAMED("Velocity Calculation");

#ifdef OPENMP
        #pragma omp parallel for schedule(static)
#endif
        for (int i = 0; i < particles.particleCount(); ++i) {
            calculateSingleVelocity(particles[i], arguments.delta_t);
        }
    }

    /**
     * @brief calculate the force for all particles
     */
    virtual void calculateForce() {
        PROFILE_ZONE_NAMED("Force Calculation");

        if(!forceCalculationEnabled)return;

        forEachDistinctParticlePair([&](Particle &par1, Particle &par2) {
            Vec3D force = forceCalculationSystem(const_cast<Args&>(arguments), par1, par2);

            // Newton 3: For every action, there is an equal and opposite reaction.
            par1.force += force;
            par2.force -= force;

#ifdef TRACY_ENABLE
            forceParticlePairsPerSecond++;
#endif
        });
    }

    /**
     * @brief Delays the position for all particles.
     */
    virtual void delayPosition() {
        PROFILE_ZONE_NAMED("Position Delay");

        forEachParticle([this](Particle &particle) {
            particle.delayPosition();
        });
    }

    /**
     * @brief Delays the force for all particles.
     */
    virtual void delayForce() {
        PROFILE_ZONE_NAMED("Force Delay");

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
    virtual void forEachParticle(const std::function<void(Particle &)> & callback) {
        particles.forEach(callback);
    }
    
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
    virtual void forEachDistinctParticlePair(const std::function<void(Particle &, Particle &)> &callback) {
        PROFILE_ZONE_NAMED("Distinct Particle Pair Iteration [DirectSum]");
        particles.forEachDistinctPair(callback);
    }
    
    /**
     * @brief Total amount of tracked particles.
     */
    virtual int particleCount() {
        return particles.particleCount();
    }
    /**
     * @brief Total amount of tracked particles.
     */
    virtual void applyGravity() {
        forEachParticle([this](Particle &particle) {
            particle.force.y += arguments.gravityFactor * particle.mass;
        });
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
        PROFILE_FUNCTION;

        std::chrono::steady_clock::time_point time_now;
        const double start = arguments.start_time;
        const double end = arguments.end_time;
        const double delta_t = arguments.delta_t;

        plotParticles(callback);

        for (double t = start; t < end; t += delta_t) {
            PROFILE_FRAME_MARK;

#ifdef TRACY_ENABLE
            time_now = std::chrono::steady_clock::now();
            auto secs = std::chrono::duration_cast<std::chrono::seconds>(time_now - lastSecondUpdate).count();

            if (secs >= 1) {
                // spdlog::debug("measure frame: {} particles updated in {} ticks", particleUpdatesPerSecond, ticksPerSecond);
                particleUpdatesPerSecond = 0;
                forceParticlePairsPerSecond = 0;
                lastSecondUpdate = time_now;
            }

            particleUpdatesPerSecond += particleCount();

            PROFILE_PLOT("Particles Per Second", particleUpdatesPerSecond);
            PROFILE_PLOT("Force Particle Pairs Per Second", forceParticlePairsPerSecond);
#endif

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
