
#include "../app/Frame.h"
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
 * @brief delay the force for all particles
 */
void Simulation::delayForce() {
    particles.forEach([](Particle &particle) {
        particle.delayForce();
    });
}

/**
 * @brief calculate the force for all particles, asserts 
 * @note naive O(n^2) implementation
 * @todo replace with efficient algorithm
 */
void Simulation::calculateForce() {
    // TODO if in debug mode, assert that all forces are zero
    // #if TODO
    // particles.forEach([](Particle &particle) {
    //     assert(particle.getForce().length() == 0 && "Forces must be zero before force calculation.");
    // });
    // #endif

    particles.forEachDistinctPair([this](Particle &particle, Particle &other) {
        Vec3D diffX = other.getPosition() - particle.getPosition();
        double distance = diffX.length();
        double mulMass = particle.getMass() * other.getMass();
        if (distance == 0.0) return;

        Vec3D force = diffX * (mulMass / (std::pow(distance, 3)));

        particle.addForce(force);
        other.addForce(-force);
    });
}
