//
// Created by mgeri on 17/01/2026.
//
#include <vector>

#include "Particle.h"
#include "ParticleContainer.h"
#include "math/Vec3.h"
#include <cmath>
#include <limits>


class Membrane {
public:
    // 2D grid of particle IDs
    std::vector<std::vector<int>> particles;

    Membrane(size_t rows, size_t cols)
            : particles(rows, std::vector<int>(cols, -1)) {
        //fills the grid with ids
        for (int i = 0; i < rows*cols; ++i) {
            particles[i / cols][i % cols] = i;
        }


    }

    void set(size_t r, size_t c, int particleID) {
        parts[r][c] = particleID;
    }

    int get(size_t r, size_t c) const {
        return parts[r][c];
    }


    /**
     * @brief calculate the  harmonic potential for all particles
     */
     double calculatePotential(const Particle& a, const Particle& b, double k, double r0) {
        double dx = a.x - b.x;
        double dy = a.y - b.y;
        double dz = a.z - b.z;
        double dist2 = dx*dx + dy*dy + dz*dz;
        double diff = dist2 - r0*r0;
        return 0.5f * k * diff * diff;
    }

    /**
 * @brief calculate the harmonic force exerted on particle a by particle b
 */
    Vec3 calculateMembraneForce(const Particle& a, const Particle& b, double k, double r0) {
        double dx = a.x - b.x;
        double dy = a.y - b.y;
        double dz = a.z - b.z;

        double dist2 = dx*dx + dy*dy + dz*dz;

        // avoid division by zero
        if (dist2 < 1e-12)
            return Vec3(0.0, 0.0, 0.0);

        // term = k * (||xi - xj||^2 - r0)
        double term = k * (dist2 - r0);

        // direction = (xj - xi) / ||xi - xj||^2
        double fx = term * (b.x - a.x) / dist2;
        double fy = term * (b.y - a.y) / dist2;
        double fz = term * (b.z - a.z) / dist2;

        return Vec3(fx, fy, fz);
    }


    void tick(){


    }
};
