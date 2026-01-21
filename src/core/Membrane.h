
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
    double stiffness = 300.0;  // Stiffness constant k
    double bondLength = 1.0;   // Average bond length r0

    /**
     * @brief Calculate forces between particle and its direct neighbors using harmonic potential.
     * Harmonic potential: U(xi, xj) = k/2 * (||xi - xj||2 - r0)^2
     * Force: F = -2 * k * (||xi - xj||2 - r0) * (xi - xj)
     * 
     * @param i Particle 1 index
     * @param j Particle 2 index
     */
    void updateForcesDirectNeighbors(size_t i, size_t j) {
        // stiffness constant
        double k = 1;

        // average bond length
        double r0 = 1;

        Particle& p_i = particleGetter(i);
        Particle& p_j = particleGetter(j);

        Vec3 delta = p_i.position - p_j.position;
        double dist = delta.length2();
        
        // Force magnitude: -2 * k * (distance - r0)
        double forceMagnitude = -2.0 * k * (dist - r0);
        
        // Force direction: normalized displacement vector
        Vec3 force = delta * (forceMagnitude / dist);
        
        // Apply Newton's third law: forces are equal and opposite
        p_i.force += force;
        p_j.force += -force;
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
