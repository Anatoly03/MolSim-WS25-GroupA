//
// Created by zhengying zhao on 24.10.25.
//

#ifndef PARTICLECONTAINER_H
#define PARTICLECONTAINER_H

#include <vector>
#include "Particle.h"
#include "math/Vec3.h"

class ParticleContainer {
private:
  std::vector<Particle> particles;

public:
  ParticleContainer() = default;

  void addParticle(const Particle& particle);

  void emplace_back(const Vec3D position, const Vec3D velocity, double mass, int type = 0);

};

#endif //PARTICLECONTAINER_H
