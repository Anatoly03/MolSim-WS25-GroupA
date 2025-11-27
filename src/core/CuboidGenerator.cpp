//
// Created by zhengying zhao on 10.11.25.
//

#include "CuboidGenerator.h"
#include "utils/MaxwellBoltzmannDistribution.h"
#include "Particle.h"

/**
 * @brief Adds a cuboid of particles to the container and applies Brownian initialization.
 *
 * This function uses a triple loop to simulate a regular 3D grid of particles.
 * A random Brownian velocity is sampled using maxwellBoltzmannDistributedVelocity().
 *
 * The Brownian velocity is added once to the cuboid's initial velocity,
 * so that Brownian motion is applied only at initialization.
 */
void addCuboid(ParticleContainer &particles, const Cuboid &cuboid, double brownian_sigma) {
    particles.reserve(particles.particleCount() + cuboid.n1 * cuboid.n2 * cuboid.n3);

    for (int i = 0; i < cuboid.n1; ++i) {
        for (int j = 0; j < cuboid.n2; ++j) {
            for (int k = 0; k < cuboid.n3; ++k) {
                Particle particle;

                particle.position = cuboid.position + Vec3D(i * cuboid.h, j * cuboid.h, k * cuboid.h);
                particle.velocity = cuboid.initial_velocity;

                if (brownian_sigma > 0.0) {
                    particle.velocity += maxwellBoltzmannDistributedVelocity(brownian_sigma, 3);
                }

                particle.mass = cuboid.mass;
                particles.add(particle);
            }
        }
    }
}
/**
 * @brief Analog to 3D version, by eliminating z-axis to keep it in 2D form.
 */
void addCuboid2D(ParticleContainer &particles, const Cuboid &cuboid, double brownian_sigma) {
    particles.reserve(particles.particleCount() + cuboid.n1 * cuboid.n2 * cuboid.n3);

    for (int i = 0; i < cuboid.n1; ++i) {
        for (int j = 0; j < cuboid.n2; ++j) {
            for (int k = 0; k < cuboid.n3; ++k) {
                Particle particle;
                
                particle.position = cuboid.position + Vec3D(i * cuboid.h, j * cuboid.h, k * cuboid.h);
                particle.velocity = cuboid.initial_velocity;

                if (brownian_sigma > 0.0) {
                    Vec3D v_brown = maxwellBoltzmannDistributedVelocity(brownian_sigma, 2);
                    v_brown.z = 0.0;
                    particle.velocity += v_brown;
                }

                particle.mass = cuboid.mass;
                particles.add(particle);
            }
        }
    }
}
