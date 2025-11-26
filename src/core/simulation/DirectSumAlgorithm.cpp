
#include "DirectSumAlgorithm.h"

#include "../math/Vec3.h"
#include "../ParticleContainer.h"
#include "../utils/ArrayUtils.h"
#include "spdlog/spdlog.h"

/**
 * @brief calculate the position for all particles
 */
void DirectSumAlgorithm::calculatePosition() {
    const double dt = arguments.delta_t;

    particles.forEach([dt](Particle &particle) {
        Vec3D x = particle.position + dt * particle.velocity + std::pow(dt, 2) * particle.force / (2 * particle.mass);
        particle.position = x;
    });
}

/**
 * @brief calculate the velocity for all particles
 */
void DirectSumAlgorithm::calculateVelocity() {
    const double dt = arguments.delta_t;

    particles.forEach([dt](Particle &particle) {
        Vec3D v = particle.velocity + dt * ((particle.force + particle.old_force) / (2 * particle.mass));
        particle.velocity = v;
    });
}

/**
 * @brief delay the force for all particles
 */
void DirectSumAlgorithm::delayForce() {
    particles.forEach([](Particle &particle) { particle.delayForce(); });
}
