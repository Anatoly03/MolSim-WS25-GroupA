
#include "../Particle.h"
#include "../utils/MaxwellBoltzmannDistribution.h"
#include "../math/Vec3.h"
#include "../math/Vec3Iter.h"
#include "MembraneGenerator.h"

/**
 * @brief Generate particles into the given container.
 */
void MembraneGenerator::generate(ParticleContainer &particles) {
    // TODO implement this
    const int dimension = dimensions();
    Membrane newMembrane(std::function<Particle&(int)>([&particles](int index) -> Particle& { return particles[index]; }));

    for (int i = 0; i < width; ++i) {
        for (int j = 0; j < height; ++j) {
            Particle particle;

            particle.position.x += i * spacing;
            particle.position.y += j * spacing;

            particle.velocity = initial_velocity;





        }

    }
}
