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
    std::vector<ParticleContainer> containers;
   
   public:
    
   public:
    typedef std::vector<ParticleContainer>::size_type size_type;
    typedef std::vector<ParticleContainer>::iterator iterator;
    typedef std::vector<ParticleContainer>::const_iterator const_iterator;

    /**
     * @brief Default constructor for LinkedCells.
     */
    LinkedCells() = default;

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

        for (const auto &container : containers) {
            total_size += container.size();
        }

        return total_size;
    }

    /**
     * @brief Add a new Particle to the cell manager.
     */
    virtual void add(const Vec3D &position, const Vec3D &velocity, double mass, int type = 0) {
        if (containers.empty()) {
            containers.emplace_back();
        }
        
        // TODO decide which container to add to
        containers[0].add(position, velocity, mass, type);
    }

    /**
     * @brief Add a new Particle to the container.
     */
    virtual void add(const Particle &particle) {
        if (containers.empty()) {
            containers.emplace_back();
        }

        containers.back().add(particle);
    }

    /**
     * @brief Reserve memory for containers.
     * @param reserve Number of containers to reserve space for.
     */
    void reserve(size_type reserve = 0) { containers.reserve(reserve); }

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
};
