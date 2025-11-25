
#pragma once

#include "../utils/Args.h"
#include "../utils/ArrayUtils.h"
#include "../writer/Writer.h"

#include "spdlog/spdlog.h"

class Simulation {
   protected:
    /**
     * @brief CLI input arguments, constant for the simulation run.
     */
    const Args arguments;

    /**
     * @brief Writer to output particle data.
     */
    std::unique_ptr<Writer> writer;

    /**
     * @brief Current simulation iteration.
     */
    int iteration = 0;

   public:
    /**
     * @note Default constructor without providing particle container is private.
     */
    Simulation() = delete;

    /**
     * @brief Default constructor
     */
    Simulation(const Args &args) : arguments(args), writer(nullptr) {}

    /**
     * @brief Destructor
     */
    virtual ~Simulation() = default;

   private:
    /**
     * @brief Plot the particles of a particular iteration to a file.
     */
    void plotParticles(int iteration) {
        if (arguments.benchmark_enabled) return;
        if (writer == nullptr) return;

        writer->plot(arguments.output_path, iteration);
    }

   public:
    /**
     * @brief Builder method to set up a writer.
     */
    void setWriter(std::unique_ptr<Writer> w) {
        spdlog::debug("writer set to `{}` format", w->getExtension());
        writer = std::move(w);
    }

   protected:
    /**
     * @brief calculate the force for two distinct particles
     */
    virtual void calculateSingleForce(Particle& p1, Particle& p2);

   public:
    /**
     * @brief Advance the simulation by one time step.
     * @note This performs one calculation step of the simulation.
     * No prints are performed and this method is benchmark viable.
     */
    virtual void tick() {}

    /**
     * @brief Run the simulation for a given time with specified time step.
     */
    virtual void run() {
        const double start = arguments.start_time;
        const double end = arguments.end_time;
        const double delta_t = arguments.delta_t;

        plotParticles(iteration);

        for (double t = start; t < end; t += delta_t) {
            tick();
            iteration++;

            if (iteration % arguments.output_interval == 0) {
                plotParticles(iteration);
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
    static std::unique_ptr<Simulation> createSimulation(ParticleContainer &particles, const Args &args);
};
