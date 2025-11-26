//
// Created by zhengying zhao on 24.10.25.
//

#pragma once

#include <functional>
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

    /**
     * @brief Default constructor for ParticleContainer.
     */
    ParticleContainer() = default;

    /**
     * @brief Copy constructor for ParticleContainer.
     */
    explicit ParticleContainer(const ParticleContainer &other) : particles(other.particles) {}

    /**
     * @brief ParticleContainer destructor.
     */
    ~ParticleContainer() = default;

    /**
     * @brief Begin non-const iterator for ParticleContainer.
     * @note Allows range-based for loops.
     * @example
     * ```c++
     * ParticleContainer container;
     *
     * for (auto &particle : container) {
     *     std::cout << particle.toString() << std::endl;
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
     *     std::cout << particle.toString() << std::endl;
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
     * @brief Get the number of particles in the container.
     */
    virtual int particleCount() const { return particles.size(); }

    /**
     * @brief Add a new Particle to the container.
     */
    virtual void add(const Particle &particle) { particles.emplace_back(particle); }

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
     *     std::cout << particle.toString() << std::endl;
     * });
     * ```
     */
    virtual void forEach(const std::function<void(Particle &)> &callback);

    // /**
    //  * @brief Reduction of an accumulator value, processing over all single particles.
    //  */
    // template<typename Acc>
    // void reduce(const std::function<Acc(Particle &, Acc)> &callback, Acc acc = default) {
    //     for (size_t i = 0; i < particles.size(); i++) {
    //         acc = callback(particles[i], acc);
    //     }
    // }

    /**
     * @brief Iteration over distinct particle pairs.
     * @param callback Function to be called for each particle pair.
     * @example
     * ```c++
     * ParticleContainer container;
     *
     * container.forEachDistinctPair([](Particle &particle1, Particle &particle2) {
     *     std::cout << particle1.toString() << " interacts with " << particle2.toString() << std::endl;
     * });
     * ```
     */
    virtual void forEachDistinctPair(const std::function<void(Particle &, Particle &)> &callback);
};
