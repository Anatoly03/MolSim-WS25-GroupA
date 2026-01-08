
#include "../Particle.h"
#include "../utils/MaxwellBoltzmannDistribution.h"
#include "../math/Vec3.h"
#include "../math/Vec3Iter.h"
#include "DiscGenerator.h"

/**
 * @brief Generate particles into the given container.
 */
void DiscGenerator::generate(ParticleContainer &particles) {
    const auto radius2 = std::pow(radius, 2.0);

    for (auto v: Vec3Iter(radius, radius, dimensions() == 2 ? 1 : radius)) {
        // check if within circle boundaries
        if (v.length2() > double(radius2)) continue;

        Particle particle;

        particle.position = center + (Vec3D)(v * spacing);
        particle.velocity = initial_velocity;

        if (brownian_sigma > 0.0) {
            particle.velocity += maxwellBoltzmannDistributedVelocity(brownian_sigma, 2);
        }

        particle.mass = mass;
        particles.add(particle);

        // permutate 
        for (auto mirror : Vec3Iter(
            v.x == 0 ? 1 : 2,
            v.y == 0 ? 1 : 2,
            v.z == 0 || dimensions() == 2 ? 1 : 2
        )) {
            Particle _particle(particle);

            if (mirror.x != 0) {
                _particle.position.x = center.x - v.x * spacing;
            }
    
            if (mirror.y != 0) {
                _particle.position.y = center.y - v.y * spacing;
            }

            if (mirror.z != 0) {
                _particle.position.z = center.z - v.z * spacing;
            }

            particles.add(_particle);
        }
    }
}
