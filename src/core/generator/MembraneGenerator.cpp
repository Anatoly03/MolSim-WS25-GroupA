
#include "../Particle.h"
#include "../utils/MaxwellBoltzmannDistribution.h"
#include "../math/Vec3.h"
#include "../math/Vec3Iter.h"
#include "MembraneGenerator.h"

/**
 * @brief Generate particles into the given container.
 */
void MembraneGenerator::generate(ParticleContainer &particles) {
    //resizing membrane id vector
    membrane.resize(size.x);
    for (int i = 0; i < size.x; i++) {
        membrane[i].resize(size.y);
    }

    size.z = 1;
    const int dimension = 2;

    for (auto v: Vec3Iter(size)) {
        Particle particle;

        particle.position = position;
        particle.position.x += v.x * spacing.x;
        particle.position.y += v.y * spacing.y;
        particle.position.z += v.z * spacing.z;

        particle.velocity = initial_velocity;

        if (brownian_sigma > 0.0) {
            particle.velocity += maxwellBoltzmannDistributedVelocity(brownian_sigma, dimension);


        }
        if (epsilon > 0.0) {
            particle.epsilon = epsilon;
        }
        if (sigma > 0.0) {
            particle.sigma = sigma;
        }

        particle.mass = mass;
        particles.add(particle);
        membrane[v.x][v.y] = particle.p_id;
    }
}
