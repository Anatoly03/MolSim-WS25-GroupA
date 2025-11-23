
#include "../utils/Args.h"
#include "../LinkedCells.h"
#include "../ParticleContainer.h"
#include "Simulation.h"
#include "LinkedCellImplementation.h"

/**
 * @brief places all particles into correct cells.
 */
void LinkedCellImplementation::placeInCells() {

    particles.forEach([&](Particle &p) {
        Vec3<int> cellIndex;

        cellIndex.x =  static_cast<int>(p.position.x) / (cellSize.x);
        cellIndex.y =  static_cast<int>(p.position.y) / cellSize.y;
        cellIndex.z =  static_cast<int>(p.position.z) / cellSize.z;

        // if (cells[cellIndex] == nullptr) {
        //     cells[cellIndex] = std::vector<Particle>();
        // }

        cells[cellIndex].push_back(&p);
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
                    k == 1 || k == nz-2)) {Vec3<int> cellIndex;

                    cellIndex.x = (int)(i) / (cellSize.x);
                    cellIndex.y = (int)(j) / cellSize.y;
                    cellIndex.z = (int)(k) / cellSize.z;
                    for (auto &p : cells[cellIndex]) {
                        result.push_back(p);
                    }

                }
            }
        }
    }
    return result;
}
std::vector<Particle*> LinkedCellImplementation::boundaryParticles() {
    std::vector<Particle*> result;
    for (int i = 1; i < nx-1; ++i) {
        for (int j = 1; j < ny-1; ++j) {
            for (int k = 1; k < nz-1; ++k) {
                if ((i == 1 || i == nx-2 ||
                      j == 1 || j == ny-2 ||
                      k == 1 || k == nz-2)) {
                    Vec3<int> cellIndex;

                    cellIndex.x = (int)(i) / (cellSize.x);
                    cellIndex.y = (int)(j) / cellSize.y;
                    cellIndex.z = (int)(k) / cellSize.z;
                    for (auto &p : cells[cellIndex]) {
                        result.push_back(p);
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
        Vec3D r = p_i.position - p_j.position;

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

        p_i.force+=(F_ij);
        p_j.force+=(-F_ij);
    });
}
void LinkedCellImplementation::calculatePosition(std::vector<Particle*>  &p) {
    const double dt = arguments.delta_t;
    for (size_t i = 0; i < p.size(); i++) {
        Vec3D x = p[i]->position + dt * p[i]->velocity + std::pow(dt, 2) * p[i]->force / (2 * p[i]->mass);
        p[i]->position = x;
    }



}
std::vector<Particle*> LinkedCellImplementation::ghostCellParticles() {
    std::vector<Particle*> result;
    for (auto &entry : cells) {
        const Vec3<int> &idx = entry.first;
        auto &cellParticles = entry.second;

        // Check if this cell is a ghost cell
        if (idx[0] == 0 || idx[0] == nx-1 ||
            idx[1] == 0 || idx[1] == ny-1 ||
            idx[2] == 0 || idx[2] == nz-1) {

            // Collect addresses of particles in this ghost cell
            for (auto &p : cellParticles) {
                result.push_back(p);
            }
        }
    }
    return result;
}

void LinkedCellImplementation::forEachBoundaryParticles(const std::function<void(Particle &)> &callback){
    std::vector<Particle*> allParticles=boundaryParticles();
    for (size_t i = 0; i < allParticles.size(); i++) {
        callback(*allParticles[i]);
    }

}
void LinkedCellImplementation::forEachInnerParticles(const std::function<void(Particle &)> &callback){
    std::vector<Particle*> allParticles=innerParticles();
    for (size_t i = 0; i < allParticles.size(); i++) {
        callback(*allParticles[i]);
    }

}
void LinkedCellImplementation::forEachGhostParticles(const std::function<void(Particle &)> &callback) {
    std::vector<Particle*> allParticles=ghostCellParticles();
    for (size_t i = 0; i < allParticles.size(); i++) {
        callback(*allParticles[i]);
    }

}
void LinkedCellImplementation::forEachDistinctPair(const std::function<void(Particle &, Particle&)> &callback) {
    for (auto &entry : cells) {

        auto &cellParticles = entry.second; // particles in this cell
        for (size_t i = 0; i < cellParticles.size(); ++i) {
            for (size_t j = i; j < cellParticles.size(); ++j) {
                if (i == j) continue;
                callback(*cellParticles[i], *cellParticles[j]);
            }
        }
    }

}
void LinkedCellImplementation::calculateVelocity() {
    const double dt = arguments.delta_t;

    particles.forEach([dt](Particle &particle) {
        Vec3D v = particle.velocity + dt * ((particle.force + particle.old_force) / (2 * particle.mass));
        particle.velocity = v;
    });
}

void LinkedCellImplementation::deleteGhostCellParticles() {
    for (auto &entry : cells) {
        const Vec3<int> &idx = entry.first;

        // Ghost cells are those at the domain boundary


        if (idx[0] == 0 || idx[0] == nx-1 ||
                    idx[1] == 0 || idx[1] == ny-1 ||
                    idx[2] == 0 || idx[2] == nz-1) {
            entry.second.clear();  // remove all particles in this ghost cell
        }
    }
}
