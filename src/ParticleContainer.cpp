//
// Created by zhengying zhao on 24.10.25.
//

#include "math/Vec3.h"
#include "ParticleContainer.h"

/**
 * Refactoring the documentation
 **/
void ParticleContainer::addParticle(const Particle& particle) {
    particles_.push_back(particle);
}

void ParticleContainer::emplace_back(const Vec3D position, const Vec3D velocity, double mass, int type) {
    particles_.emplace_back(position, velocity, mass, type);
}
