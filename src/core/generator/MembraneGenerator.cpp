
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
            particle.isMembrane = true;
            particle.p_id = index;
            particle.mass = mass;
            particles.add(particle);
            newMembrane.particles[i][j] = index;
            index++;


        }

    }


    //adds the neighbors of the particles
    for (int i = 0; i < width; ++i) {
        for (int j = 0; j < height; ++j) {

            int currentIndex = newMembrane.particles[i][j];
            Particle &p = particles[currentIndex];

            // Loop over neighbor offsets (-1, 0, +1)
            for (int di = -1; di <= 1; ++di) {
                for (int dj = -1; dj <= 1; ++dj) {

                    // Skip the particle itself
                    if (di == 0 && dj == 0) continue;

                    int ni = i + di;
                    int nj = j + dj;

                    // Check bounds
                    if (ni >= 0 && ni < width && nj >= 0 && nj < height) {
                        //std::cout<<"MemGen trigger "<<std::endl;

                        p.neighborParticles.push_back((newMembrane.particles[ni][nj]));
                    }
                }
            }
        }
    }




    particles.addMembrane(newMembrane);

}
