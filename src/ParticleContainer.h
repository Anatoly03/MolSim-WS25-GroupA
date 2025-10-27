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
 *
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

    iterator begin() { return particles.begin(); }
    iterator end() { return particles.end(); }
    const_iterator begin() const { return particles.begin(); }
    const_iterator end() const { return particles.end(); }

    void clear() { particles.clear(); }
    void reserve(size_type capacity) { particles.reserve(capacity); }

    size_type size() const { return particles.size(); }
    size_type capacity() const { return particles.capacity(); }

    void emplace_back(const Vec3D position, const Vec3D velocity, double mass, int type = 0) {
        particles.emplace_back(position, velocity, mass, type);
    }

    void pushback(const Particle &p) { particles.push_back(p); }
    void pushback(Particle &&p) { particles.emplace_back(std::move(p)); }

    /**
     * @brief Iteration over single particles.
     */
    void forEach(const std::function<void(Particle &)> &callback) {
        for (size_t i = 0; i < particles.size(); i++) {
            callback(particles[i]);
        }
    }

    /**
     * @brief Iteration over distinct particle pairs.
     */
    void forEachPair(const std::function<void(Particle &, Particle &)> &callback) {
        for (size_t i = 0; i < particles.size(); i++) {
            for (size_t j = i + 1; j < particles.size(); j++) {
                if (i == j) continue;
                callback(particles[i], particles[j]);
            }
        }
    }
};
