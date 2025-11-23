
#include "../utils/Args.h"
#include "../LinkedCells.h"
#include "../ParticleContainer.h"
#include "Simulation.h"
#include "LinkedCellImplementation.h"

/**
 * @brief places all particles into correct cells.
 */
void LinkedCellImplementation::placeInCells(ParticleContainer &particles) {
    particles.forEach([&](Particle &p) {
        int cx = static_cast<int>(p.getPosition().x / cellSize);
        int cy = static_cast<int>(p.getPosition().y / cellSize);
        int cz = static_cast<int>(p.getPosition().z / cellSize);

        int index = cz * (p.getPosition().x * p.getPosition().y) + cy * p.getPosition().x + cx;
        cells[index].push_back(p);
    });



}
std::vector<Particle*> LinkedCellImplementation::boundaryParticles() {
    std::vector<Particle*> result;
    for (int i = 1; i < nx-1; ++i) {
        for (int j = 1; j < ny-1; ++j) {
            for (int k = 1; k < nz-1; ++k) {
                if (i == 1 || i == nx-2 ||
                    j == 1 || j == ny-2 ||
                    k == 1 || k == nz-2) {
                    for (auto &p : cells[i + nx * (j + ny * k)]) {
                        result.push_back(&p);
                    }

                }
            }
        }
    }
    return result;
}

/**
 * @brief calculate the position for all particles
 */
void LinkedCellImplementation::calculatePosition() {
    const double dt = arguments.delta_t;

    particles.forEach([dt](Particle &particle) {
        Vec3D x = particle.getPosition() + dt * particle.getVelocity() +
                  std::pow(dt, 2) * particle.getForce() / (2 * particle.getMass());
        particle.setPosition(x);
    });
}

/**
 * @brief calculate the velocity for all particles
 */
void LinkedCellImplementation::calculateVelocity() {
    const double dt = arguments.delta_t;

    particles.forEach([dt](Particle &particle) {
        Vec3D v =
                particle.getVelocity() + dt * ((particle.getForce() + particle.getOldForce()) / (2 * particle.getMass()));
        particle.setVelocity(v);
    });
}

/**
 * @brief delay the force for all particles
 */
void LinkedCellImplementation::delayForce() {
    particles.forEach([](Particle &particle) {
        particle.delayForce();
    });
}
std::vector<Particle*> LinkedCellImplementation::innerParticles() {
    std::vector<Particle*> result;
    for (int i = 1; i < nx-1; ++i) {
        for (int j = 1; j < ny-1; ++j) {
            for (int k = 1; k < nz-1; ++k) {
                if (!(i == 1 || i == nx-2 ||
                    j == 1 || j == ny-2 ||
                    k == 1 || k == nz-2)) {
                    for (auto &p : cells[i + nx * (j + ny * k)]) {
                        result.push_back(&p);
                    }

                }
            }
        }
    }
    return result;
}

void LinkedCellImplementation::calculateForce() {
    // Lennard-Jones Parameters (hard code for now)
    const double epsilon = 5.0;
    const double sigma   = 1.0;

    particles.forEachDistinctPair([&](Particle &p_i, Particle &p_j) {
        Vec3D r = p_i.getPosition() - p_j.getPosition();

        double r_len = r.length();
        if (r_len == 0.0) {
            return;
        }
        if(r_len>cutOff){
            return;
        }
        // rf. formula (3)
        double inv_r2 = 1.0 / (r_len * r_len);
        double sigma_r2 = sigma * inv_r2;
        double scalar = -24.0 * epsilon * std::pow(inv_r2, 2) *
                        ( std::pow(sigma_r2, 6) - 2.0 * std::pow(sigma_r2, 12) );
        Vec3D F_ij = scalar * r;

        p_i.addForce(F_ij);
        p_j.addForce(-F_ij);
    });
}