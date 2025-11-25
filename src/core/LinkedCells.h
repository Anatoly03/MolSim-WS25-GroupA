//
// Created by zhengying zhao on 24.10.25.
//

#pragma once

#include <functional>
#include <vector>

#include "Particle.h"
#include "ParticleContainer.h"
#include "math/Vec3.h"

/**
 * @class LinkedCells
 * @brief Manager of ParticleContainers, dividing the space into isolated physical cells.
 */
class LinkedCells {
   private:
    /**
     * @brief Internal storage of particle containers.
     */
    std::map<Vec3I, std::vector<Particle>> containers;

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
    
   public:
    typedef std::map<Vec3I, std::vector<Particle> >::size_type size_type;
    typedef std::map<Vec3I, std::vector<Particle> >::iterator iterator;
    typedef std::map<Vec3I, std::vector<Particle> >::const_iterator const_iterator;

    /**
     * @brief Default constructor for LinkedCells.
     */
    LinkedCells() = default;

    /**
     * @brief Constructor for LinkedCells specifying cell size.
     */
    LinkedCells(Vec3I cellSize) : cellSize(cellSize) {}

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
    virtual size_type size() const {
        int total_size = 0;

        for (const auto &it : containers) {
            const auto &container = it.second;
            total_size += container.size();
        }

        return total_size;
    }

    /**
     * @brief Absorb a particle container and sort particles into cells.
     */
    virtual void absorb(ParticleContainer &particles) {
        particles.forEach([&](Particle &p) {
            add(p);
        });
    }

    /**
     * @brief Add a new Particle to the cell manager.
     */
    virtual void add(Particle &particle) {
        auto cellIndex = Vec3I(
            (int) std::floor(particle.position.x / (double) cellSize.x),
            (int) std::floor(particle.position.y / (double) cellSize.y),
            (int) std::floor(particle.position.z / (double) cellSize.z)
        );

        containers[cellIndex].emplace_back(particle);
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
     * @brief Iteration over single particles.
     * @param callback Function to be called for each particle.
     * @example
     * ```c++
     * LinkedCells cells;
     *
     * cells.forEach([](Particle &particle) {
     *     std::cout << particle.toString() << std::endl;
     * });
     * ```
     */
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

};
