#include "LinkedCells.h"

/**
 * Iteration over containers, each over single particles using a callback function.
 */
void LinkedCells::forEach(const std::function<void(Particle &)> &callback) {
    for (size_t i = 0; i < containers.size(); i++) {
        containers[i].forEach(callback);
    }
}

/**
 * Iteration over containers, each over distinct particle pairs using a callback function.
 */
void LinkedCells::forEachDistinctPair(const std::function<void(Particle &, Particle &)> &callback) {
    for (size_t i = 0; i < containers.size(); i++) {
        containers[i].forEachDistinctPair(callback);
    }
}
