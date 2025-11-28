
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

    for (auto v: Vec3Iter(radius, radius, 1)) {
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

        // reflect into first quadrant
        if (v.x != 0) {
            Particle _particle(particle);
            _particle.position.x = center.x - v.x * spacing;
            particles.add(_particle);
        }

        // reflect into third quadrant
        if (v.y != 0) {
            Particle _particle(particle);
            _particle.position.y = center.y - v.y * spacing;
            particles.add(_particle);
        }

        // reflect into fourth quadrant
        if (v.x != 0 && v.y != 0) {
            Particle _particle(particle);
            _particle.position.x = center.x - v.x * spacing;
            _particle.position.y = center.y - v.y * spacing;
            particles.add(_particle);
        }
    }
}
