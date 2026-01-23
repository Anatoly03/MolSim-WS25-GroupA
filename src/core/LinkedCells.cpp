#include "LinkedCells.h"
#include "math/Util.h"
#include "math/Vec3Iter.h"

#include "spdlog/spdlog.h"
#include <fmt/format.h>
#include <omp.h>

// (omp.h already included at file scope)

/**
* iterate over all cells and removes out of range
*/
int LinkedCells::clearOutOfBoundsCells() {
    std::vector<Vec3I> cellsToRemove;

    for (auto &it : containers) {
        const Vec3I &cellIndex = it.first;

        if (!ascending(domainMin.x, cellIndex.x, domainMax.x)
            || !ascending(domainMin.y, cellIndex.y, domainMax.y)
            || !ascending(domainMin.z, cellIndex.z, domainMax.z)) {
            cellsToRemove.emplace_back(cellIndex);
            continue;
        }


        auto &particles = it.second;
        //checks for periodic boundaries
        if(boarderXmin == 2 ||
           boarderXmax == 2 ||
           boarderYmin == 2 ||
           boarderYmax == 2 ||
           boarderZmin == 2 ||
           boarderZmax == 2) {
            for (auto pIt = particles.begin(); pIt != particles.end();) {
                Particle &p = particleGetter(*pIt);
                if (p.position.x < domainMin.x && boarderXmin == 2) {
                    p.position.x += (domainMax.x - domainMin.x);
                } else if (p.position.x > domainMax.x && boarderXmax == 2) {
                    p.position.x -= (domainMax.x - domainMin.x);
                }
                if (p.position.y < domainMin.y && boarderYmin == 2) {
                    p.position.y += (domainMax.y - domainMin.y);
                } else if (p.position.y > domainMax.y && boarderYmax == 2) {
                    p.position.y -= (domainMax.y - domainMin.y);
                }
                if (p.position.z < domainMin.z && boarderZmin == 2) {
                    p.position.z += (domainMax.z - domainMin.z);
                } else if (p.position.z > domainMax.z && boarderZmax == 2) {
                    p.position.z -= (domainMax.z - domainMin.z);
                }
                Vec3I newIndex = getIndex(p);
                if (newIndex != cellIndex) {
                    //removes particle from old cell and add into new cell
                    containers[newIndex].push_back(*pIt);
                    pIt = particles.erase(pIt);
                } else {
                    ++pIt;
                }


            }
        }else{
            particles.erase(
                    std::remove_if(particles.begin(), particles.end(),
                                   [&](int particleIndex) {
                                       const Particle &p = particleGetter(particleIndex);
                                       return (p.position.x < domainMin.x || p.position.x > domainMax.x ||
                                               p.position.y < domainMin.y || p.position.y > domainMax.y ||
                                               p.position.z < domainMin.z || p.position.z > domainMax.z);
                                   }),
                    particles.end()
            );
        }
    }


    for (const auto &cellIndex : cellsToRemove) {
        containers.erase(cellIndex);
    }

    return static_cast<int>(cellsToRemove.size());
}


/**
* Iteration over containers, each over single particles using a callback function.
*/
void LinkedCells::forEach(const std::function<void(Particle &)> &callback) {
#ifdef OPENMP
    // Parallelize over cells by creating tasks for each cell.
    #pragma omp parallel
    #pragma omp single nowait
    {
        for (auto &it : containers) {
            #pragma omp task firstprivate(it)
            {
                auto &particles = it.second;
                for (auto &particleIndex: particles) {
                    callback(particleGetter(particleIndex));
                }
            }
        }
        #pragma omp taskwait
    }
#else
    for (auto &it : containers) {
        auto &particles = it.second;
        for (auto &particleIndex: particles) callback(particleGetter(particleIndex));
    }
#endif
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
                callback(particleGetter(particles[i]), particleGetter(particles[j]));
            }
        }

// only iterate over neighbouring chunks with greater chunk index (avoid duplication)
        for (const Vec3I indexDelta : Vec3Iter(CHUNK_RADIUS)) {
            const Vec3I nIndex = cellIndex + indexDelta; // neighbour index

            if (indexDelta.length() == 0) continue;
            if (!ascending(domainMin.x, nIndex.x, domainMax.x)) continue;
            if (!ascending(domainMin.y, nIndex.y, domainMax.y)) continue;
            if (!ascending(domainMin.z, nIndex.z, domainMax.z)) continue;

            auto neighbourIt = containers.find(nIndex);
            if (neighbourIt == containers.end()) continue;
            auto &neighbour = neighbourIt->second;

// calculate all distinct pairs across chunks
            for (size_t i = 0; i < particles.size(); i++) {
                for (size_t j = 0; j < neighbour.size(); j++) {
                    callback(particleGetter(particles[i]), particleGetter(neighbour[j]));
                }
            }
        }
    }
}


/**
 * Iteration over distinct particle pairs yielding raw particle indices.
 * This variant can be parallelized more easily by callers.
 */
void LinkedCells::forEachDistinctPairIndexed(const std::function<void(int, int)> &callback) {
    const int CHUNK_RADIUS = 2;

// OpenMP header included at file scope when available (via CMake)

    // Collect keys to avoid concurrent map iteration issues when parallelizing
    std::vector<Vec3I> keys;
    keys.reserve(containers.size());
    for (const auto &it : containers) {
        const Vec3I &cellIndex = it.first;
        if (!ascending(domainMin.x, cellIndex.x, domainMax.x)) continue;
        if (!ascending(domainMin.y, cellIndex.y, domainMax.y)) continue;
        if (!ascending(domainMin.z, cellIndex.z, domainMax.z)) continue;
        keys.push_back(cellIndex);
    }

#ifdef OPENMP
    #pragma omp parallel for schedule(runtime)
    for (int k = 0; k < static_cast<int>(keys.size()); k++) {
        const Vec3I cellIndex = keys[k];
        auto &particles = containers[cellIndex];

        // pairs within same cell
        for (size_t i = 0; i < particles.size(); i++) {
            for (size_t j = i + 1; j < particles.size(); j++) {
                if (i == j) continue;
                callback(particles[i], particles[j]);
            }
        }

        // neighbouring cells
        for (const Vec3I indexDelta : Vec3Iter(CHUNK_RADIUS)) {
            const Vec3I nIndex = cellIndex + indexDelta;
            if (indexDelta.length() == 0) continue;
            if (!ascending(domainMin.x, nIndex.x, domainMax.x)) continue;
            if (!ascending(domainMin.y, nIndex.y, domainMax.y)) continue;
            if (!ascending(domainMin.z, nIndex.z, domainMax.z)) continue;

            auto neighbourIt = containers.find(nIndex);
            if (neighbourIt == containers.end()) continue;
            auto &neighbour = neighbourIt->second;

            for (size_t i = 0; i < particles.size(); i++) {
                for (size_t j = 0; j < neighbour.size(); j++) {
                    callback(particles[i], neighbour[j]);
                }
            }
        }
    }
#else
    for (const auto &it : containers) {
        const Vec3I &cellIndex = it.first;
        auto &particles = it.second;

        if (!ascending(domainMin.x, cellIndex.x, domainMax.x)) continue;
        if (!ascending(domainMin.y, cellIndex.y, domainMax.y)) continue;
        if (!ascending(domainMin.z, cellIndex.z, domainMax.z)) continue;

        for (size_t i = 0; i < particles.size(); i++) {
            for (size_t j = i + 1; j < particles.size(); j++) {
                if (i == j) continue;
                callback(particles[i], particles[j]);
            }
        }

        for (const Vec3I indexDelta : Vec3Iter(CHUNK_RADIUS)) {
            const Vec3I nIndex = cellIndex + indexDelta; // neighbour index

            if (indexDelta.length() == 0) continue;
            if (!ascending(domainMin.x, nIndex.x, domainMax.x)) continue;
            if (!ascending(domainMin.y, nIndex.y, domainMax.y)) continue;
            if (!ascending(domainMin.z, nIndex.z, domainMax.z)) continue;

            auto neighbourIt = containers.find(nIndex);
            if (neighbourIt == containers.end()) continue;
            auto &neighbour = neighbourIt->second;

            for (size_t i = 0; i < particles.size(); i++) {
                for (size_t j = 0; j < neighbour.size(); j++) {
                    callback(particles[i], neighbour[j]);
                }
            }
        }
    }
#endif
}

/**
* @brief wrapper for 'for each bordered' without relative chunk
*/
void LinkedCells::forEachBordered(const std::function<void(Particle &)> &callback) {
    forEachBordered([this, &callback](Particle &p, Vec3I /*ghostCellIndex*/) {
        callback(p);
    });
}

/**
* @brief Iteration over the cells at domain border.
* @note The same particle CAN AND WILL be processed multiple times for small
* domain size. This is because particles at edges and corners belong to multiple
* border planes.
* @details
*
* ```
*                NORTH      BACK
*
*               /- - - - - -/
*             /           / |
*           /    XZ     /   |   EAST
*  WEST    |- - - - - -| YZ |
*          |           |   /
*          |     XY    | /
*          |- - - - - -|
*
*   FRONT      SOUTH
* ```
*
* Iterate over six planes of cells.
*/
void LinkedCells::forEachBordered(const std::function<void(Particle &, Vec3I)> &callback) {
    const auto domainSize = domainMax - domainMin + Vec3I(1);

// XY PLANE [FRONT]
//std::cout<<boarderXmin<<std::endl;
    if(boarderZmin==1) {
//std::cout<<"minZ"<<std::endl;
        for (auto xyPlane: Vec3Iter(domainSize.x, domainSize.y, 1)) {
            Vec3I cellIndex = domainMin + Vec3I(xyPlane.x, xyPlane.y, 0);
            auto &particles = containers[cellIndex];

            for (auto &p: particles) {
                callback(particleGetter(p), cellIndex + Vec3I(0, 0, -1));
            }
        }
    }else{
/*for (auto xyPlane: Vec3Iter(domainSize.x, domainSize.y, 1)) {
           Vec3I cellIndex = domainMin + Vec3I(xyPlane.x, xyPlane.y, 0);
           containers.erase(cellIndex);
       }*/

       clearOutOfBoundsCells();
    }

// XY PLANE [BACK]
    if(boarderZmax == 1) {
        for (auto xyPlane: Vec3Iter(domainSize.x, domainSize.y, 1)) {
            Vec3I cellIndex = domainMin + Vec3I(xyPlane.x, xyPlane.y, domainSize.z - 1);
            auto &particles = containers[cellIndex];

            for (auto &p: particles) {
                callback(particleGetter(p), cellIndex + Vec3I(0, 0, 1));
            }
        }
    }else{
/*for (auto xyPlane: Vec3Iter(domainSize.x, domainSize.y, 1)) {
           Vec3I cellIndex = domainMin + Vec3I(xyPlane.x, xyPlane.y, domainSize.z - 1);
           containers.erase(cellIndex);
       }*/

        clearOutOfBoundsCells();
    }

// XZ PLANE [NORTH]
    if(boarderYmin== 1) {
        for (auto xyPlane: Vec3Iter(domainSize.x, 1, domainSize.z)) {
            Vec3I cellIndex = domainMin + Vec3I(xyPlane.x, 0, xyPlane.z);
            auto &particles = containers[cellIndex];

            for (auto &p: particles) {
                callback(particleGetter(p), cellIndex + Vec3I(0, -1, 0));
            }
        }
    }else{
/*for (auto xyPlane: Vec3Iter(domainSize.x, 1, domainSize.z)) {
           Vec3I cellIndex = domainMin + Vec3I(xyPlane.x, 0, xyPlane.z);
           containers.erase(cellIndex);
       }*/

            clearOutOfBoundsCells();
    }

// XZ PLANE [SOUTH]
    if(boarderYmax == 1) {
        for (auto xyPlane: Vec3Iter(domainSize.x, 1, domainSize.z)) {
            Vec3I cellIndex = domainMin + Vec3I(xyPlane.x, domainSize.y - 1, xyPlane.z);
            auto &particles = containers[cellIndex];

            for (auto &p: particles) {
                callback(particleGetter(p), cellIndex + Vec3I(0, 1, 0));
            }
        }
    }else{
/*for (auto xyPlane: Vec3Iter(domainSize.x, 1, domainSize.z)) {
           Vec3I cellIndex = domainMin + Vec3I(xyPlane.x, domainSize.y - 1, xyPlane.z);
           containers.erase(cellIndex);
       }*/

       clearOutOfBoundsCells();
    }

// YZ PLANE [WEST]
    if(boarderXmin == 1) {
        for (auto xyPlane: Vec3Iter(1, domainSize.y, domainSize.z)) {
            Vec3I cellIndex = domainMin + Vec3I(0, xyPlane.y, xyPlane.z);
            auto &particles = containers[cellIndex];

            for (auto &p: particles) {
                callback(particleGetter(p), cellIndex + Vec3I(-1, 0, 0));
            }
        }
    }else{
/*for (auto xyPlane: Vec3Iter(1, domainSize.y, domainSize.z)) {
           Vec3I cellIndex = domainMin + Vec3I(0, xyPlane.y, xyPlane.z);
           containers.erase(cellIndex);
       }*/
        //std::cout<<"minX"<<std::endl;
        //std::cout<<"minX"<<std::endl;

            clearOutOfBoundsCells();
    }

// YZ PLANE [EAST]
    if(boarderXmax == 1) {
        for (auto xyPlane: Vec3Iter(1, domainSize.y, domainSize.z)) {
            Vec3I cellIndex = domainMin + Vec3I(domainSize.x - 1, xyPlane.y, xyPlane.z);
            auto &particles = containers[cellIndex];

            for (auto &p: particles) {
                callback(particleGetter(p), cellIndex + Vec3I(1, 0, 0));
            }
        }
    }else{
/*for (auto xyPlane : Vec3Iter(1, domainSize.y, domainSize.z)) {
           Vec3I cellIndex = domainMin + Vec3I(domainSize.x - 1, xyPlane.y, xyPlane.z);
           containers.erase(cellIndex);
       }*/

        clearOutOfBoundsCells();
    }
}

/**
* @brief Iterates over all particles and updates their parent chunk when they leave their cell.
*/
void LinkedCells::reindex() {
    for (auto &it : containers) {
        auto &particles = it.second;

        for (size_t i = 0; i < particles.size(); i++) {
            Particle &p = particleGetter(particles[i]);

            const auto currentCellIndex = it.first;
            const auto newCellIndex = getIndex(p);

            if (!ascending(domainMin.x, newCellIndex.x, domainMax.x)
                || !ascending(domainMin.y, newCellIndex.y, domainMax.y)
                || !ascending(domainMin.z, newCellIndex.z, domainMax.z)) {
// the line below breaks github CI
// spdlog::warn("Particle {} at {} left cell domain {} -> {}", p.p_id, p.position, currentCellIndex, newCellIndex);
// spdlog::debug("Cell size {}, Current cell {}, New cell {}", cellSize, currentCellIndex, newCellIndex);
                continue;
            }

            if (newCellIndex != currentCellIndex) {
// add to new container
                containers[newCellIndex].emplace_back(particles[i]);

// remove from current container
                particles.erase(particles.begin() + i);
                i--;

                spdlog::trace("Particle reindexed!");
            }
        }
    }
}