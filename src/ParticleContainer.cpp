//
// Created by zhengying zhao on 24.10.25.
//

 /**
  * Refactoring the documentation
  **/
  void ParticleContainer::addParticle(const Particle& particle) {
    particles.push_back(particle);
  }

  void ParticleContainer::emplace_back(const Vec3D position, const Vec3D velocity, double mass, int type) {
    particles.emplace_back(position, velocity, mass, type);
  }


