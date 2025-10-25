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

    void addParticle(const Particle& particle) { particles_.push_back(particle);}
    void addParticle(Particle&& particle) { particles_.emplace_back(std::move(particle));}

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

  void pushback(const Particle& particle) { particles_.push_back(particle);}
  void pushback(Particle&& particle) { particles_.emplace_back(std::move(particle));}

  /**
   * iteration for single particles
   */
  template <typename P>
  void forEachParticle(P&& p) {
    for (auto& particle : particles_) {
      p(particle);
    }
  }

  /**
   * iteration for particle pairs
   */
  template <typename P>
  void forEachParticlePair(P&& p) {
   for (size_t i = 0; i < particles_.size(); ++i) {
     for (size_t j = i+1; j < particles_.size(); ++j) {
       p(particles_[i], particles_[j]);
     }
   }
  }
  template <typename P>
  void forEachParticlePair(P&& p) const{
    for (size_t i = 0; i < particles_.size(); ++i) {
      for (size_t j = i+1; j < particles_.size(); ++j) {
        p(particles_[i], particles_[j]);
      }
    }
  }

  iterator begin() { return particles_.begin(); }
  iterator end() { return particles_.end(); }

  void addParticle(const Particle& particle) { particles_.push_back(particle);}
  void addParticle(Particle&& particle) { particles_.emplace_back(std::move(particle));}

  void emplace_back(const Vec3D position, const Vec3D velocity, double mass, int type = 0) {
    particles_.emplace_back(position, velocity, mass, type);
  }

  void pushback(const Particle& particle) { particles_.push_back(particle);}
  void pushback(Particle&& particle) { particles_.emplace_back(std::move(particle));}

  /**
   * iteration for single particles
   */
  template <typename P>
  void forEachParticle(P&& p) {
    for (auto& particle : particles_) {
      p(particle);
    }
  }

  /**
   * iteration for particle pairs
   */
  template <typename P>
  void forEachParticlePair(P&& p) {
   for (size_t i = 0; i < particles_.size(); ++i) {
     for (size_t j = i+1; j < particles_.size(); ++j) {
       p(particles_[i], particles_[j]);
     }
   }
  }
  template <typename P>
  void forEachParticlePair(P&& p) const{
    for (size_t i = 0; i < particles_.size(); ++i) {
      for (size_t j = i+1; j < particles_.size(); ++j) {
        p(particles_[i], particles_[j]);
      }
    }
  }

private:
  container_t particles_;
};
