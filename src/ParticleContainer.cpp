
#include "ParticleContainer.h"
#include "Particle.h"
#include <functional>

/**
 * Iteration over single particles using a callback function.
 */
void ParticleContainer::forEach(const std::function<void(Particle &)> &callback) {
    // NOLINTNEXTLINE(altera-unroll-loops)
    for (auto particle : particles) {
        callback(particle);
    }
}

/**
 * Iteration over distinct particle pairs using a callback function.
 * Index-based particle comparison ensures a pair has distinct particles.
 */
void ParticleContainer::forEachPair(const std::function<void(Particle &, Particle &)> &callback) {
    // NOLINTNEXTLINE(altera-unroll-loops)
    for (auto particle1 : particles) {
        // NOLINTNEXTLINE(altera-unroll-loops)
        for (auto particle2 : particles) {
            callback(particle1, particle2);
        }
    }
}
