#include "LinkedCells.h"
#include "math/Util.h"
#include "math/Vec3Iter.h"

#include "spdlog/spdlog.h"
#include <fmt/format.h>

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




        particles.erase(
                std::remove_if(particles.begin(), particles.end(),
                               [&](const Particle &p) {
                                   return (p.position.x < domainMin.x || p.position.x > domainMax.x ||
                                           p.position.y < domainMin.y || p.position.y > domainMax.y ||
                                           p.position.z < domainMin.z || p.position.z > domainMax.z);
                               }),
                particles.end()
        );
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
    for (auto &it : containers) {
        const Vec3I &cellIndex = it.first;
        auto &particles = it.second;

        if (!ascending(domainMin.x, cellIndex.x, domainMax.x)
            || !ascending(domainMin.y, cellIndex.y, domainMax.y)
            || !ascending(domainMin.z, cellIndex.z, domainMax.z)) {
            std::string tmp;

            for (auto &p: particles) {
                if (tmp.length() > 0) tmp += ", ";
                tmp += p.toString();
            }

            {
                auto idx_str = fmt::format("({},{},{})", it.first.x, it.first.y, it.first.z);
                spdlog::warn("cell {} out of domain bounds, affecting {} particles: {}", idx_str, particles.size(), tmp);
            }
        }

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

/**
* @brief wrapper for 'for each bordered' without relative chunk
*/
void LinkedCells::forEachBordered(const std::function<void(Particle &)> &callback) {
    forEachBordered([&callback](Particle &p, Vec3I /*ghostCellIndex*/) {
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
    if(boarderZmin=="reflect") {
//std::cout<<"minZ"<<std::endl;
        for (auto xyPlane: Vec3Iter(domainSize.x, domainSize.y, 1)) {
            Vec3I cellIndex = domainMin + Vec3I(xyPlane.x, xyPlane.y, 0);
            auto &particles = containers[cellIndex];

            for (auto &p: particles) {
                callback(p, cellIndex + Vec3I(0, 0, -1));
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
    if(boarderZmax == "reflect") {
        for (auto xyPlane: Vec3Iter(domainSize.x, domainSize.y, 1)) {
            Vec3I cellIndex = domainMin + Vec3I(xyPlane.x, xyPlane.y, domainSize.z - 1);
            auto &particles = containers[cellIndex];

            for (auto &p: particles) {
                callback(p, cellIndex + Vec3I(0, 0, 1));
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
    if(boarderYmin=="reflect") {
        for (auto xyPlane: Vec3Iter(domainSize.x, 1, domainSize.z)) {
            Vec3I cellIndex = domainMin + Vec3I(xyPlane.x, 0, xyPlane.z);
            auto &particles = containers[cellIndex];

            for (auto &p: particles) {
                callback(p, cellIndex + Vec3I(0, -1, 0));
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
    if(boarderYmax=="reflect") {
        for (auto xyPlane: Vec3Iter(domainSize.x, 1, domainSize.z)) {
            Vec3I cellIndex = domainMin + Vec3I(xyPlane.x, domainSize.y - 1, xyPlane.z);
            auto &particles = containers[cellIndex];

            for (auto &p: particles) {
                callback(p, cellIndex + Vec3I(0, 1, 0));
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
    if(boarderXmin=="reflect") {
        for (auto xyPlane: Vec3Iter(1, domainSize.y, domainSize.z)) {
            Vec3I cellIndex = domainMin + Vec3I(0, xyPlane.y, xyPlane.z);
            auto &particles = containers[cellIndex];

            for (auto &p: particles) {
                callback(p, cellIndex + Vec3I(-1, 0, 0));
            }
        }
    }else{
/*for (auto xyPlane: Vec3Iter(1, domainSize.y, domainSize.z)) {
           Vec3I cellIndex = domainMin + Vec3I(0, xyPlane.y, xyPlane.z);
           containers.erase(cellIndex);
       }*/
        std::cout<<"minX"<<std::endl;
        //std::cout<<"minX"<<std::endl;
        clearOutOfBoundsCells();
    }

// YZ PLANE [EAST]
    if(boarderXmax=="reflect") {
        for (auto xyPlane: Vec3Iter(1, domainSize.y, domainSize.z)) {
            Vec3I cellIndex = domainMin + Vec3I(domainSize.x - 1, xyPlane.y, xyPlane.z);
            auto &particles = containers[cellIndex];

            for (auto &p: particles) {
                callback(p, cellIndex + Vec3I(1, 0, 0));
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
            Particle &p = particles[i];

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
                containers[newCellIndex].emplace_back(p);

// remove from current container
                particles.erase(particles.begin() + i);
                i--;

                spdlog::trace("Particle reindexed!");
            }
        }
    }
}