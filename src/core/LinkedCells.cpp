#include "LinkedCells.h"

/**
 * Iteration over single particles using a callback function.
 */
void LinkedCells::forEach(const std::function<void(Particle &)> &callback) {
    for (size_t i = 0; i < containers.size(); i++) {
        containers[i].forEach(callback);
    }
}

/**
 * Iteration over distinct particle pairs using a callback function.
 * Index-based particle comparison ensures a pair has distinct particles.
 */
void LinkedCells::forEachDistinctPair(const std::function<void(Particle &, Particle &)> &callback) {
    for (size_t i = 0; i < containers.size(); i++) {
        containers[i].forEachDistinctPair(callback);
    }
}
