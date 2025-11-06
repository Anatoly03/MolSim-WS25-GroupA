
#include "Simulation.h"

#include "../app/Frame.h"
#include "FileReader.h"
#include "ParticleContainer.h"
#include "spdlog/spdlog.h"
#include "utils/ArrayUtils.h"
#include "writer/Writer.h"

/**
 * @brief calculate the position for all particles
 */
void Simulation::calculatePosition() {
    const double dt = arguments.delta_t;

    particles.forEach([dt](Particle &particle) {
        Vec3D x = particle.getPosition() + dt * particle.getVelocity() +
                  std::pow(dt, 2) * particle.getForce() / (2 * particle.getMass());
        particle.setPosition(x);
    });
}

/**
 * @brief calculate the velocity for all particles
 */
void Simulation::calculateVelocity() {
    const double dt = arguments.delta_t;

    particles.forEach([dt](Particle &particle) {
        Vec3D v =
            particle.getVelocity() + dt * ((particle.getForce() + particle.getOldForce()) / (2 * particle.getMass()));
        particle.setVelocity(v);
    });
}

/**
 * @brief calculate the force for all particles
 */
void Simulation::calculateForce() {
    particles.forEach([this](Particle &particle) {
        Vec3D force(0);

        particles.forEach([&particle, &force](const Particle &other) {
            if (other == particle) return;

            Vec3D diffX = other.getPosition() - particle.getPosition();
            double distance = diffX.length();
            double mulMass = particle.getMass() * other.getMass();
            if (distance == 0.0) return;

            force += diffX * (mulMass / (std::pow(distance, 3)));
        });

        particle.delayForce();
        particle.setForce(force);
    });
}

/**
 * @brief Run the simulation.
 */
void Simulation::run() {
    double current_time = arguments.start_time;
    int iteration = 0;

    plotParticles(iteration);

    // for this loop, we assume: current x, current f and current v are known
    while (current_time < arguments.end_time) {
        calculatePosition();
        calculateForce();
        calculateVelocity();

        iteration++;
        if (iteration % 10 == 0) {
            plotParticles(iteration);
        }
        spdlog::info("Iteration {} finished.", iteration);

        current_time += arguments.delta_t;
    }

    spdlog::info("Output written. Terminating...");
}
