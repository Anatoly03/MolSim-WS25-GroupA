//
// Created by zhengying zhao on 24.10.25.
//

#pragma once

#include <functional>
#include <vector>

#include "Particle.h"
#include "ParticleContainer.h"
#include "math/Vec3.h"
#include <cmath>
#include <limits>

/**
 * @class LinkedCells
 * @brief Manager of ParticleContainers, dividing the space into isolated physical cells.
 */
class LinkedCells {
   private:
    /**
     * @brief Internal storage of particle indeces that are managed by cell.
     */
    std::map<Vec3I, std::vector<int>> containers;

   public:
    /**
     * @brief Typedef for particle getter function
     */
    typedef std::function<Particle&(int)> get_particle;

   private:
    /**
     * @brief Method to get particle reference from index. Provided by parent.
     */
    get_particle particleGetter;

   public:
    /**
     * @brief cell divison size
     */
    Vec3I cellSize = Vec3I(10);

    /**
     * @brief cell divison size
     */
    Vec3I domainMin = Vec3I(0);

    /**
     * @brief cell divison size
     */
    Vec3I domainMax = Vec3I(0);
    typedef std::map<Vec3I, std::vector<int> >::size_type size_type;
    typedef std::map<Vec3I, std::vector<int> >::iterator iterator;
    typedef std::map<Vec3I, std::vector<int> >::const_iterator const_iterator;

    int boarderXmin;
    int boarderXmax;
    int boarderYmin;
    int boarderYmax;
    int boarderZmin;
    int boarderZmax;

    ParticleContainer particleContainer;

    /**
     * @brief Constructor for LinkedCells specifying particle getter and cell size.
     */
    /*LinkedCells(ParticleContainer& particleContainer1, Vec3I cellSize) :  cellSize(cellSize),particleContainer(particleContainer1) {
        if (cellSize.x == 0 || cellSize.y == 0 || cellSize.z == 0) {
            spdlog::error("cell size for linked cells was set to zero-volume: ({},{},{})", cellSize.x, cellSize.y, cellSize.z);
        }
    }*/

    LinkedCells(get_particle getter, Vec3I cellSize) : particleGetter(getter), cellSize(cellSize) {
        if (cellSize.x == 0 || cellSize.y == 0 || cellSize.z == 0) {
            spdlog::error("cell size for linked cells was set to zero-volume: ({},{},{})", cellSize.x, cellSize.y, cellSize.z);
        }
    }

    /**
     * @brief Copy constructor for LinkedCells.
     */
    LinkedCells(const LinkedCells &other) = default;

    /**
     * @brief LinkedCells destructor.
     */
    ~LinkedCells() = default;

    /**
     * @brief Begin non-const iterator for LinkedCells.
     * @note Allows range-based for loops.
     * @example
     * ```c++
     * LinkedCells cells;
     *
     * for (auto &container : cells) {
     *     for (auto &particle : container) {
     *         std::cout << container.toString() << std::endl;
     *     }
     * }
     * ```
     */
    iterator begin() { return containers.begin(); }

    /**
     * @brief End non-const iterator for LinkedCells.
     * @note Allows range-based for loops.
     */
    iterator end() { return containers.end(); }

    /**
     * @brief Begin const iterator for LinkedCells.
     * @note Allows range-based for loops over constant elements.
     * @example
     * ```c++
     * LinkedCells cells;
     *
     * for (const auto &container : cells) {
     *     for (const auto &particle : container) {
     *         std::cout << container.toString() << std::endl;
     *     }
     * }
     * ```
     */
    const_iterator begin() const { return containers.begin(); }

    /**
     * @brief End const iterator for LinkedCells.
     * @note Allows range-based for loops.
     */
    const_iterator end() const { return containers.end(); }

    /**
     * @brief Get the number of total particles in all cells combined.
     */
    virtual int particleCount() const {
        int total_size = 0;

        for (const auto &it : containers) {
            const auto &container = it.second;
            total_size += container.size();
        }

        return total_size;
    }

    /**
     * @brief Get the number of total active cells.
     */
    virtual int cellCount() const {
        return containers.size();
    }

    /**
     * @brief Absorb a particle container and sort particles into cells.
     */
    virtual void absorb(ParticleContainer &particles) {
        particles.forEachIndexed([&](Particle &p, int index) {
            add(p, index);
        });


    }

    /**
     * @brief Add a new Particle to the cell manager.
     * @returns Index of the cell.
     */
    virtual Vec3I add(Particle &p, int particleId) {
        auto cellIndex = getIndex(p);
        containers[cellIndex].emplace_back(particleId);
        return cellIndex;
    }

    /**
     * @brief Set the domain boundaries and return domain size.
     * @note Range is inclusive.
     */
    virtual Vec3I setDomainSize(Vec3I min, Vec3I max) {
        domainMin = min;
        domainMax = max;

        return domainMax - domainMin;
    }

    /**
     * @brief set border types.
     * @note Range is inclusive.
     */
    virtual void setBorder(int boarderXmin, int boarderXmax, int boarderYmin, int boarderYmax, int boarderZmin, int boarderZmax) {
        this->boarderXmin = boarderXmin;
        this->boarderXmax = boarderXmax;
        this->boarderYmin = boarderYmin;
        this->boarderYmax = boarderYmax;
        this->boarderZmin = boarderZmin;
        this->boarderZmax = boarderZmax;
    }

    /**
     * @brief Removes out of bounds cells.
     * @returns amount of removed cells
     * @note Range is inclusive.
     */
    virtual int clearOutOfBoundsCells();

    // /**
    //  * @brief Iteration over single particles.
    //  * @param callback Function to be called for each particle.
    //  * @example
    //  * ```c++
    //  * LinkedCells cells;
    //  *
    //  * cells.forEach([](Particle &particle) {
    //  *     std::cout << particle.toString() << std::endl;
    //  * });
    //  * ```
    //  */
    virtual void forEach(const std::function<void(Particle &)> &callback);

    /**
     * @brief Iteration over distinct particle pairs per cell.
     * @param callback Function to be called for each particle pair.
     * @example
     * ```c++
     * LinkedCells cells;
     *
     * cells.forEachDistinctPair([](Particle &particle1, Particle &particle2) {
     *     std::cout << particle1.toString() << " interacts with " << particle2.toString() << std::endl;
     * });
     * ```
     */
    virtual void forEachDistinctPair(const std::function<void(Particle &, Particle &)> &callback);

    /**
     * @brief Iteration over single particles which are located in the cells near the domain border.
     * @param callback Function to be called for each particle.
     */
    virtual void forEachBordered(const std::function<void(Particle &)> &callback);

    /**
     * @brief Iteration over single particles which are located in the cells near the domain border.
     * @param callback Function to be called for each particle and the chunk index of the closest ghost cell.
     */
    virtual void forEachBordered(const std::function<void(Particle &, Vec3I)> &callback);

    /**
     * @brief Iterates over all particles and updates their parent chunk.
     */
    virtual void reindex();

    /**
     * @brief Divides particle position by cell size to get the cell index.
     */
    Vec3I getIndex(Particle &p){
        Vec3I cellIndex;

        cellIndex.x = static_cast<int>(std::floor(p.position.x / static_cast<double>(cellSize.x)));
        cellIndex.y = static_cast<int>(std::floor(p.position.y / static_cast<double>(cellSize.y)));
        cellIndex.z = static_cast<int>(std::floor(p.position.z / static_cast<double>(cellSize.z)));
        
        // spdlog::trace("{} | {} ----> {}", p.position, cellSize, cellIndex);

        return cellIndex;
    }
};
