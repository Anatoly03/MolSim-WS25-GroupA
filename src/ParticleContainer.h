//
// Created by zhengying zhao on 24.10.25.
//

#pragma once

#include <vector>
#include "Particle.h"
#include "math/Vec3.h"

/**
 * @class ParticleContainer
 * @brief Container for Particle with simple iteration over particles and particle pairs
 */
class ParticleContainer {
public:
    // std::vector<Particle> particles;
    using container_t = std::vector<Particle>;
    using value_type = Particle;
    using size_type = container_t::size_type;
    using iterator = container_t::iterator;
    using const_iterator = container_t::const_iterator;

    ParticleContainer() = default;

    explicit ParticleContainer(size_type capacity) {
        particles_.reserve(capacity);
    }

    iterator begin() { return particles_.begin(); }
    iterator end() { return particles_.end(); }
    const_iterator begin() const { return particles_.begin(); }
    const_iterator end() const { return particles_.end(); }

    void addParticle(const Particle& particle);

    void emplace_back(const Vec3D position, const Vec3D velocity, double mass, int type = 0);

    void pushback(const Particle& particle);

    /**
     * @brief Get the number of particles in the container.
     * @return Number of particles. Equivalent description: Number of elements.
     */
    size_type size() const {
        return particles_.size();
    }

private:
  container_t particles_;
};
