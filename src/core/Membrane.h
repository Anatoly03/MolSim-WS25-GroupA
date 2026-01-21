
#pragma once

#include <functional>
#include <vector>

#include "Particle.h"
#include "ParticleContainer.h"
#include "math/Vec3.h"
#include <cmath>
#include <limits>

class Membrane {
   private:
    // two dimensional sheet of particles
    std::vector<std::vector<int>> particles;

   public:
    /**
     * @brief Typedef for particle getter function
     */
    typedef std::function<Particle&(int)> get_particle;

   public:

    /**
     * @brief Constructor specifying particle getter.
     */
    LinkedCells(get_particle getter) : particleGetter(getter) {

    }

    /**
     * @brief Membrane destructor.
     */
    ~Membrane() = default;
}
