
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
    Membrane(get_particle getter) : particleGetter(getter) {

    }

    /**
     * @brief Membrane destructor.
     */
    ~Membrane() = default;

    /**
     * @brief Update forces for all particles in the membrane.
     * Iterates over all particles and calculates forces from direct and diagonal neighbors.
     */
    void updateForce() {
        int width = particles.size();

        for (size_t x = 1; x < width; ++x) {
            for (size_t y = 1; y < particles[x].size(); ++y) {
                int par = particles[x][y];

                updateForcesDirectNeighbors(par, particles[x-1][y]);
                updateForcesDirectNeighbors(par, particles[x][y-1]);
                updateForcesDiagonalNeighbors(par, particles[x-1][y-1]);

                if (x + 1 < width) {
                    updateForcesDirectNeighbors(par, particles[x+1][y]);
                }
            }
        }
    }

   private:
    get_particle particleGetter;

    /**
     * @brief Calculate forces between particle and its direct neighbors.
     * 
     * @param i Particle 1
     * @param j Particle 2
     */
    void updateForcesDirectNeighbors(size_t i, size_t j) {
        // TODO: Implementation
    }

    /**
     * @brief Calculate forces between particle and its diagonal neighbors.
     * 
     * @param i Particle 1
     * @param j Particle 2
     */
    void updateForcesDiagonalNeighbors(size_t i, size_t j) {
        // TODO: Implementation
    }
}
