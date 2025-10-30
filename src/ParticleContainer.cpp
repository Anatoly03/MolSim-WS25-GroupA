
#include "ParticleContainer.h"

/**
 * Iteration over single particles using a callback function.
 */
void ParticleContainer::forEach(const std::function<void(Particle &)> &callback) {
    for (size_t i = 0; i < particles.size(); i++) {
        callback(particles[i]);
    }
}

/**
 * Iteration over distinct particle pairs using a callback function.
 * Index-based particle comparison ensures a pair has distinct particles.
 */
void ParticleContainer::forEachPair(const std::function<void(Particle &, Particle &)> &callback) {
    for (size_t i = 0; i < particles.size(); i++) {
        for (size_t j = i + 1; j < particles.size(); j++) {
            if (i == j) continue;
            callback(particles[i], particles[j]);
        }
    }
}