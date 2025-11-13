
#include "Simulation.h"

#include "../app/Frame.h"
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
 * @brief delay the force for all particles
 */
void Simulation::delayForce() {
    particles.forEach([](Particle &particle) { particle.delayForce(); });
}

/**
 * @brief Calculates the force for all particles, using Lennard-Jones forces.
 *
 * This function implements a pairwise force computation using the Lennard-Jones
 * potential, which are applied symmetrically to both particles in the pair,
 * following Newton’s third law.
 *
 * @note naive O(n^2) implementation
 */
// TODO replace with efficient algorithm
void Simulation::calculateForce() {
    // Lennard-Jones Parameters (hard code for now)
    const double epsilon = 5.0;
    const double sigma   = 1.0;

    particles.forEachDistinctPair([&](Particle &particle, Particle &other) {
        Vec3D r = particle.getPosition() - other.getPosition();
        double distance = r.length();

        if (distance == 0.0) {
            return;
        }

        // rf. formula (3)
        double inv_r2 = 1.0 / std::pow(distance, 2);
        double sigma_r2 = sigma * inv_r2;
        double scalar = -24.0 * epsilon * std::pow(inv_r2, 2) *
                        (std::pow(sigma_r2, 6) - 2.0 * std::pow(sigma_r2, 12) );
        Vec3D force = scalar * r;

        particle.addForce(force);
        other.addForce(-force);
    });
}



