//
// Created by zhengying zhao on 10.11.25.
//

#include "CuboidGenerator.h"
#include "utils/MaxwellBoltzmannDistribution.h"
#include "Particle.h"
#include "math/Vec3.h"
#include "math/Vec3Iter.h"

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
    for (auto v: Vec3Iter(cuboid.size)) {
        Particle particle;

        particle.position = cuboid.position;
        particle.position.x += v.x * cuboid.spacing.x;
        particle.position.y += v.y * cuboid.spacing.y;
        particle.position.z += v.z * cuboid.spacing.z;

        particle.velocity = cuboid.initial_velocity;

        if (brownian_sigma > 0.0) {
            particle.velocity += maxwellBoltzmannDistributedVelocity(brownian_sigma, 3);
        }

        particle.mass = cuboid.mass;
        particles.add(particle);
    }
}
