
#include "../Particle.h"
#include "../utils/MaxwellBoltzmannDistribution.h"
#include "../math/Vec3.h"
#include "../math/Vec3Iter.h"
#include "CuboidGenerator.h"

/**
 * @brief Generate particles into the given container.
 */
void CuboidGenerator::generate(ParticleContainer &particles) {
    const int dimension = size.z == 1 ? 2 : 3;
    
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

        particle.mass = mass;
        particles.add(particle);
    }
}
