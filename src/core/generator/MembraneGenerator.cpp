
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
    int index = particles.particleCount();

    Membrane newMembrane(std::function<Particle&(int)>([&particles](int index) -> Particle& { return particles[index]; }), width,height,arguments);
    newMembrane.stiffness = this->stiffness;
    newMembrane.bond_length = this->bond_length;

    for (int i = 0; i < width; ++i) {
        for (int j = 0; j < height; ++j) {
            Particle particle;
            particle.position.x = center.x + i * spacing;
            particle.position.y =  center.y + j * spacing;

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
            newMembrane.particles[i][j] = index;
            index++;


        }

    }

}
