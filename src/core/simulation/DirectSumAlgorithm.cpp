
#include "../utils/ArrayUtils.h"
#include "../ParticleContainer.h"
#include "../writer/Writer.h"
#include "Simulation.h"
#include "DirectSumAlgorithm.h"

#include "spdlog/spdlog.h"

/**
 * @brief calculate the position for all particles
 */
void DirectSumAlgorithm::calculatePosition() {
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
void DirectSumAlgorithm::calculateVelocity() {
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
void DirectSumAlgorithm::delayForce() {
    particles.forEach([](Particle &particle) {
        particle.delayForce();
    });
}

/**
 * @brief Calculates the Lennard-Jones force acting on all particles.
 *
 * This function implements a pairwise force computation using the Lennard-Jones potential,
 * which are applied symmetrically to both particles in the pair,
 * following Newton’s third law.
 *
 * @note naive O(n^2) implementation
 * @todo replace with efficient algorithm
 * @param None (operates on member variable @c particles)
 */
void DirectSumAlgorithm::calculateForce() {
    // Lennard-Jones Parameters (hard code for now)
    const double epsilon = 5.0;
    const double sigma   = 1.0;

    particles.forEachDistinctPair([&](Particle &p_i, Particle &p_j) {
        Vec3D r = p_i.getPosition() - p_j.getPosition();
        double r_len = r.length();
        if (r_len == 0.0) {
            return;
        }

        // rf. formula (3)
        double inv_r2 = 1.0 / (r_len * r_len);
        double sigma_r2 = sigma * inv_r2;
        double scalar = -24.0 * epsilon * std::pow(inv_r2, 2) *
                        ( std::pow(sigma_r2, 6) - 2.0 * std::pow(sigma_r2, 12) );
        Vec3D F_ij = scalar * r;

        p_i.addForce(F_ij);
        p_j.addForce(-F_ij);
    });
}
