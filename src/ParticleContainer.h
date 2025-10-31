//
// Created by zhengying zhao on 24.10.25.
//

#pragma once

#include <vector>

#include "Particle.h"
#include "math/Vec3.h"

/**
 * @class ParticleContainer
 * @brief Refactoring,owning container for Particle with simple iteration over particles and particle pairs
 */
class ParticleContainer {
   private:
    /**
     * Internal storage of particles.
     */
    std::vector<Particle> particles;

   public:
    typedef std::vector<Particle>::size_type size_type;
    typedef std::vector<Particle>::iterator iterator;
    typedef std::vector<Particle>::const_iterator const_iterator;

    ParticleContainer() = default;
    ~ParticleContainer() = default;

    explicit ParticleContainer(size_type capacity) { particles.reserve(capacity); }

    /**
     * @brief Begin non-const iterator for ParticleContainer.
     * @note Allows range-based for loops.
     * @example
     * ```c++
     * ParticleContainer container;
     *
     * for (auto &particle : container) {
     *     std::cout << particle.toString() << "\n";
     * }
     * ```
     */
    iterator begin() { return particles.begin(); }

    /**
     * @brief End non-const iterator for ParticleContainer.
     * @note Allows range-based for loops.
     */
    iterator end() { return particles.end(); }

    /**
     * @brief Begin const iterator for ParticleContainer.
     * @note Allows range-based for loops over constant elements.
     * @example
     * ```c++
     * ParticleContainer container;
     *
     * for (const auto &particle : container) {
     *     std::cout << particle.toString() << "\n";
     * }
     * ```
     */
    const_iterator begin() const { return particles.begin(); }

    /**
     * @brief End const iterator for ParticleContainer.
     * @note Allows range-based for loops.
     */
    const_iterator end() const { return particles.end(); }

    /**
     * @brief Clear all particles from the container.
     */
    void clear() { particles.clear(); }

    /**
     * @brief Get the number of particles in the container.
     */
    size_type size() const { return particles.size(); }

    /**
     * @brief Get the capacity of the undderlying vector in the container.
     */
    size_type capacity() const { return particles.capacity(); }

    /**
     * @brief Add a new Particle to the container.
     */
    void emplace_back(const Vec3D &position, const Vec3D &velocity, double mass, int type = 0) {
        particles.emplace_back(position, velocity, mass, type);
    }

    /**
     * @brief Add a new Particle to the container.
     */
    void emplace_back(const Particle &particle) { particles.emplace_back(particle); }

    /**
     * @brief Reserve memory for particles.
     * @param reserve Number of particles to reserve space for.
     */
    void reserve(size_type reserve = 0) { particles.reserve(reserve); }

    /**
     * @brief Iteration over single particles.
     * @param callback Function to be called for each particle.
     * @example
     * ```c++
     * ParticleContainer container;
     *
     * container.forEach([](Particle &particle) {
     *     std::cout << particle.toString() << "\n";
     * });
     * ```
     */
    void forEach(const std::function<void(Particle &)> &callback);

    // /**
    //  * @brief Reduction of an accumulator value, processing over all single particles.
    //  */
    // template<typename Acc>
    // void reduce(const std::function<Acc(Particle &, Acc)> &callback, Acc acc = default) {
    //     for (auto particle : particles) {
    //         acc = callback(particle, acc);
    //     }
    // }

    /**
     * @brief Iteration over distinct particle pairs.
     * @param callback Function to be called for each particle pair.
     * @example
     * ```c++
     * ParticleContainer container;
     *
     * container.forEachPair([](Particle &particle1, Particle &particle2) {
     *     std::cout << particle1.toString() << " interacts with " << particle2.toString() << "\n";
     * });
     * ```
     */
    void forEachPair(const std::function<void(Particle &, Particle &)> &callback);
};
