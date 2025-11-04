
#pragma once

#include "../app/Frame.h"
#include "FileReader.h"
#include "ParticleContainer.h"
#include "utils/ArrayUtils.h"
#include "writer/Writer.h"

class Simulation {
   private:
    Writer writer;
    Args arguments;
    ParticleContainer particles;

   public:
    /**
     * @note Default constructor without providing particle container is private.
     */
    Simulation() = delete;
    // Simulation() : particles(), arguments(), writer(particles) {}

    /**
     * @brief Default constructor
     */
    Simulation(const ParticleContainer &p, const Args &args) : particles(p), arguments(args), writer(particles) {}

    /**
     * @brief Builder method to set up a writer.
     */
    void setWriter(Writer &w) { writer = w; }

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
     * @brief calculate the force for all particles
     */
    void calculateForce();

    /**
     * @brief Plot the particles of a particular iteration to a file.
     */
    void plotParticles(int iteration) {
        std::string out_name(arguments.output_path);
        writer.plot(out_name, iteration);
    }

   public:
    /**
     * @brief Run the simulation for a given time with specified time step.
     * @param end_time Total simulation time.
     * @param delta_t Time step delta.
     */
    void run();
};
