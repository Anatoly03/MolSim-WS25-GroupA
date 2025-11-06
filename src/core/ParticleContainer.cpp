#include <functional>

#include "ParticleContainer.h"
#include "Particle.h"

/**
 * Iteration over single particles using a callback function.
 */
void ParticleContainer::forEach(const std::function<void(Particle &)> &callback) {
    // NOLINTNEXTLINE(altera-unroll-loops)
    for (size_t i = 0; i < particles.size(); i++)  {
        callback(particle);
    }
}

/**
 * Iteration over distinct particle pairs using a callback function.
 * Index-based particle comparison ensures a pair has distinct particles.
 */
void ParticleContainer::forEachDistinctPair(const std::function<void(Particle &, Particle &)> &callback) {
    // NOLINTNEXTLINE(altera-unroll-loops)
    for (size_t i = 0; i < particles.size(); i++) {
        // NOLINTNEXTLINE(altera-unroll-loops)
        for (size_t j = i + 1; j < particles.size(); j++) {
            if (i == j) continue;
            callback(particles[i], particles[j]);
        }
    }
}
