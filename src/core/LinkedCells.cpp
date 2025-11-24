#include "LinkedCells.h"
#include "math/Util.h"
#include "math/Vec3Iter.h"

/**
 * Iteration over containers, each over single particles using a callback function.
 */
void LinkedCells::forEach(const std::function<void(Particle &)> &callback) {
    for (auto &it : containers) {
        auto &particles = it.second;

        for (auto &p: particles) {
            callback(p);
        }
    }
}

/**
 * Iteration over close distinct particle pairs using a callback function. Particles
 * across some chunks are ignored.
 */
void LinkedCells::forEachDistinctPair(const std::function<void(Particle &, Particle &)> &callback) {
    const int CHUNK_RADIUS = 2;

    for (auto &it : containers) {
        const Vec3I &cellIndex = it.first;
        auto &particles = it.second;

        if (!ascending(domainMin.x, cellIndex.x, domainMax.x)) continue;
        if (!ascending(domainMin.y, cellIndex.y, domainMax.y)) continue;
        if (!ascending(domainMin.z, cellIndex.z, domainMax.z)) continue;

        // calculate all distinct pairs within same chunk
        for (size_t i = 0; i < particles.size(); i++) {
            for (size_t j = i + 1; j < particles.size(); j++) {
                if (i == j) continue;
                callback(particles[i], particles[j]);
            }
        }

        // only iterate over neighbouring chunks with greater chunk index (avoid duplication)
        for (const Vec3I indexDelta : Vec3Iter(CHUNK_RADIUS)) {
            const Vec3I nIndex = cellIndex + indexDelta; // neighbour index

            if (indexDelta.length() == 0) continue;
            if (!ascending(domainMin.x, nIndex.x, domainMax.x)) continue;
            if (!ascending(domainMin.y, nIndex.y, domainMax.y)) continue;
            if (!ascending(domainMin.z, nIndex.z, domainMax.z)) continue;

            auto &neighbour = containers[nIndex];

            // calculate all distinct pairs across chunks
            for (size_t i = 0; i < particles.size(); i++) {
                for (size_t j = 0; j < neighbour.size(); j++) {
                    callback(particles[i], neighbour[j]);
                }
            }
        }
    }
}
