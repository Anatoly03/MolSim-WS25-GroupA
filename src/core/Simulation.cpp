
#include "../app/Frame.h"
#include "FileReader.h"
#include "utils/ArrayUtils.h"
#include "ParticleContainer.h"
#include "writer/Writer.h"
#include "Simulation.h"

#include "spdlog/spdlog.h"

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
