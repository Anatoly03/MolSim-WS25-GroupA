
#pragma once

#include "../app/Frame.h"
#include "FileReader.h"
#include "ParticleContainer.h"
#include "spdlog/spdlog.h"
#include "utils/ArrayUtils.h"
#include "writer/Writer.h"

class Simulation {
   private:
    ParticleContainer particles;
    Args arguments;
    std::unique_ptr<Writer> writer;

   public:
    /**
     * @brief Current simulation iteration.
     */
    int iteration = 0;

   public:
    /**
     * @note Default constructor without providing particle container is private.
     */
    Simulation() = delete;
    // Simulation() : particles(), arguments(), writer(particles) {}

    /**
     * @brief Default constructor
     */
    Simulation(const ParticleContainer &p, const Args &args) : particles(p), arguments(args), writer(nullptr) {}

    /**
     * @brief Builder method to set up a writer.
     */
    void setWriter(std::unique_ptr<Writer> w) {
        spdlog::debug("writer set to `{}` format", w->getExtension());
        writer = std::move(w);
    }

    /**
     * @brief Destructor
     */
    ~Simulation() = default;

   protected:
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

    /**
     * @brief Plot the particles of a particular iteration to a file.
     */
    void plotParticles(int iteration) {
        if (arguments.benchmark_enabled) return;
        if (writer == nullptr) return;

        std::string out_name(arguments.output_path);
        writer->plot(out_name, iteration);
    }

   public:
    /**
     * @brief Advance the simulation by one time step.
     * @note This performs position, force, and velocity calculations.
     * No prints are performed and this method is benchmark viable.
     */
    void tick() {
        calculatePosition();
        delayForce();
        calculateForce();
        calculateVelocity();

        iteration++;
    }

    /**
     * @brief Run the simulation for a given time with specified time step.
     */
    void run() {
        const double start = arguments.start_time;
        const double end = arguments.end_time;
        const double delta_t = arguments.delta_t;

        plotParticles(iteration);

        for (double t = start; t < end; t += delta_t) {
            tick();

            if (iteration % 10 == 0) {
                plotParticles(iteration);
            }

            spdlog::debug("Iteration {} finished.", iteration);
        }

        spdlog::info("Output written. Terminating...");
    }
};
