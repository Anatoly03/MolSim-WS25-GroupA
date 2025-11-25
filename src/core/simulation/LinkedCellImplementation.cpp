
#include "../utils/Args.h"
#include "../LinkedCells.h"
#include "../ParticleContainer.h"
#include "Simulation.h"
#include "LinkedCellImplementation.h"

#include <spdlog/spdlog.h>

/**
 * @brief force calculation logic for two particles
 */

/**
 * @brief calculate the position for all particles
 */
void LinkedCellImplementation::calculatePosition() {
    const double dt = arguments.delta_t;

    cells.forEach([dt](Particle &particle) {
        Vec3D x = particle.position + dt * particle.velocity + std::pow(dt, 2) * particle.force / (2 * particle.mass);
        particle.position = x;
    });
}

/**
 * @brief calculate the velocity for all particles
 */
void LinkedCellImplementation::calculateVelocity() {
    const double dt = arguments.delta_t;

    cells.forEach([dt](Particle &particle) {
        Vec3D v = particle.velocity + dt * ((particle.force + particle.old_force) / (2 * particle.mass));
        particle.velocity = v;
    });
}

/**
 * @brief delay the force for all particles
 */
void LinkedCellImplementation::delayForce() {
    cells.forEach([](Particle &particle) { particle.delayForce(); });
}

/**
 * @brief Calculates the Lennard-Jones force acting on all particles.
 *
 * This function implements a pairwise force computation using the Lennard-Jones potential,
 * which are applied symmetrically to both particles in the pair,
 * following Newton’s third law.
 *
 * @note naive O(n^2) implementation
 * @todo replace with efficient algorithm
 * @param None (operates on member variable @c particles)
 */
void LinkedCellImplementation::calculateForce(Particle& p_i, Particle& p_j) {
    // TODO lennard-Jones Parameters (hard code for now)
    const double epsilon = 5.0;
    const double sigma = 1.0;

    Vec3D r = p_i.position - p_j.position;
    double r_len = r.length();
    if (r_len == 0.0) {
        return;
    }

    // rf. formula (3)
    double inv_r2 = 1.0 / (r_len * r_len);
    double sigma_r2 = sigma * inv_r2;
    double scalar = -24.0 * epsilon * std::pow(inv_r2, 2) * (std::pow(sigma_r2, 6) - 2.0 * std::pow(sigma_r2, 12));
    Vec3D F_ij = scalar * r;

    p_i.force += F_ij;
    p_j.force -= F_ij;
}

void LinkedCellImplementation::reindexParticles() {
    cells.reindex();
};

void LinkedCellImplementation::calculateBorderBehaviour() {
    cells.forEachBordered([&](Particle &p, Vec3I /* ghostCellIndex */) {
        Particle ghost(p);

        // TODO reflecting boundary condition

        calculateForce(p, ghost);
    });
}

// /**
//  * @brief places all particles into correct cells using the cell index
//  */
// void LinkedCellImplementation::placeInCells(ParticleContainer &particles) {
//     particles.forEach([&](Particle p) {
//         cells[getIndex(p)].push_back(p);
//     });
// }

// /**
//  * @brief delay the force for all particles
//  */
// void LinkedCellImplementation::delayForce() {
//     particles.forEach([](Particle &particle) {
//         particle.delayForce();
//     });
// }

// std::vector<Particle*> LinkedCellImplementation::innerParticles() {
//     std::vector<Particle*> result;

//     for (int i = 1; i < nx-1; ++i) {
//         for (int j = 1; j < ny-1; ++j) {
//             for (int k = 1; k < nz-1; ++k) {
//                 if (!(i == 1 || i == nx-2 ||
//                     j == 1 || j == ny-2 ||
//                     k == 1 || k == nz-2)) {
//                         Vec3<int> cellIndex;

//                     cellIndex.x = i;
//                     cellIndex.y = j;
//                     cellIndex.z = k;

//                     for (auto &p : cells[cellIndex]) {
//                         result.push_back(p);
//                     }

//                 }
//             }
//         }
//     }

//     return result;
// }

// /**
//  * 
//  */
// std::vector<Particle*> LinkedCellImplementation::boundaryParticles() {
//     std::vector<Particle*> result;

//     for (int i = 1; i < nx-1; ++i) {
//         for (int j = 1; j < ny-1; ++j) {
//             for (int k = 1; k < nz-1; ++k) {
//                 if ((i == 1 || i == nx-2 ||
//                       j == 1 || j == ny-2 ||
//                       k == 1 || k == nz-2)) {
//                     Vec3<int> cellIndex;

//                     cellIndex.x = (int)(i);
//                     cellIndex.y = (int)(j);
//                     cellIndex.z = (int)(k);
//                     for (auto &p : cells[cellIndex]) {
//                         result.push_back(p);
//                     }

//                 }
//             }
//         }
//     }
//     return result;
// }

// /**
//  * 
//  */
// void LinkedCellImplementation::calculateForce() {
//     // Lennard-Jones Parameters (hard code for now)
//     const double epsilon = 5.0;
//     const double sigma   = 1.0;

//     particles.forEachDistinctPair([&](Particle &p_i, Particle &p_j) {
//         Vec3D r = p_i.position - p_j.position;

//         double r_len = r.length();
//         if (r_len == 0.0) {
//             return;
//         }
//         if(r_len>cutOff){
//             return;
//         }
//         // rf. formula (3)
//         double inv_r2 = 1.0 / (r_len * r_len);
//         double sigma_r2 = sigma * inv_r2;
//         double scalar = -24.0 * epsilon * std::pow(inv_r2, 2) *
//                         ( std::pow(sigma_r2, 6) - 2.0 * std::pow(sigma_r2, 12) );
//         Vec3D F_ij = scalar * r;

//         p_i.force+=(F_ij);
//         p_j.force+=(-F_ij);
//     });
// }


// /**
//  * 
//  */
// void LinkedCellImplementation::calculatePosition(std::vector<Particle*>  &p) {
//     const double dt = arguments.delta_t;
//     for (size_t i = 0; i < p.size(); i++) {
//         Vec3D x = p[i]->position + dt * p[i]->velocity + std::pow(dt, 2) * p[i]->force / (2 * p[i]->mass);
//         p[i]->position = x;
//     }
// }

// /**
//  * 
//  */
// std::vector<Particle*> LinkedCellImplementation::ghostCellParticles() {
//     std::vector<Particle*> result;
//     for (auto &entry : cells) {
//         const Vec3<int> &idx = entry.first;
//         auto &cellParticles = entry.second;


//         if (idx[0] == 0 || idx[0] == nx-1 ||
//             idx[1] == 0 || idx[1] == ny-1 ||
//             idx[2] == 0 || idx[2] == nz-1) {


//             for (auto &p : cellParticles) {
//                 result.push_back(p);
//             }
//         }
//     }
//     return result;
// }

// /**
//  * 
//  */
// void LinkedCellImplementation::forEachBoundaryParticles(const std::function<void(Particle &)> &callback){
//     std::vector<Particle*> allParticles=boundaryParticles();
//     for (size_t i = 0; i < allParticles.size(); i++) {
//         callback(*allParticles[i]);
//     }
// }

// /**
//  * 
//  */
// void LinkedCellImplementation::forEachInnerParticles(const std::function<void(Particle &)> &callback){
//     std::vector<Particle*> allParticles=innerParticles();
//     for (size_t i = 0; i < allParticles.size(); i++) {
//         callback(*allParticles[i]);
//     }
// }

// /**
//  * 
//  */
// void LinkedCellImplementation::forEachGhostParticles(const std::function<void(Particle &)> &callback) {
//     std::vector<Particle*> allParticles=ghostCellParticles();
//     for (size_t i = 0; i < allParticles.size(); i++) {
//         callback(*allParticles[i]);
//     }
// }

// /**
//  * 
//  */
// void LinkedCellImplementation::forEachDistinctPair(const std::function<void(Particle &, Particle&)> &callback) {
//     for (auto &entry : cells) {

//         auto &cellParticles = entry.second;
//         for (size_t i = 0; i < cellParticles.size(); ++i) {
//             for (size_t j = i; j < cellParticles.size(); ++j) {
//                 if (i == j) continue;
//                 callback(*cellParticles[i], *cellParticles[j]);
//             }
//         }
//     }
// }

// /**
//  * 
//  */
// void LinkedCellImplementation::calculateVelocity() {
//     const double dt = arguments.delta_t;

//     particles.forEach([dt](Particle &particle) {
//         Vec3D v = particle.velocity + dt * ((particle.force + particle.old_force) / (2 * particle.mass));
//         particle.velocity = v;
//     });
// }

// /**
//  * 
//  */
// void LinkedCellImplementation::deleteGhostCellParticles() {
//     for (auto &entry : cells) {
//         const Vec3<int> &idx = entry.first;

//         if (idx[0] == 0 || idx[0] == nx-1 ||
//                     idx[1] == 0 || idx[1] == ny-1 ||
//                     idx[2] == 0 || idx[2] == nz-1) {
//             entry.second.clear();
//         }
//     }
// }

// /**
//  * 
//  */
// void LinkedCellImplementation::setMinMax() {
//     double minX = +std::numeric_limits<double>::infinity();
//     double minY = +std::numeric_limits<double>::infinity();
//     double minZ = +std::numeric_limits<double>::infinity();

//     particles.forEach([&](Particle &p) {
//         minX = std::min(minX, p.position.x);
//         minY = std::min(minY, p.position.y);
//         minZ = std::min(minZ, p.position.z);
//     });
//     domainMin.x = minX - cellSize.x;
//     domainMin.y = minY - cellSize.y;
//     domainMin.z = minZ - cellSize.z;

//     double maxX = -std::numeric_limits<double>::infinity();
//     double maxY = -std::numeric_limits<double>::infinity();
//     double maxZ = -std::numeric_limits<double>::infinity();

//     particles.forEach([&](Particle &p) {
//         maxX = std::max(maxX, p.position.x);
//         maxY = std::max(maxY, p.position.y);
//         maxZ = std::max(maxZ, p.position.z);
//     });

//     domainMax.x = maxX + cellSize.x;
//     domainMax.y = maxY + cellSize.y;
//     domainMax.z = maxZ + cellSize.z;
// }

// /**
//  * @brief divides particle position by 
//  */
// Vec3<int> LinkedCellImplementation::getIndex(Particle &p){
//     Vec3<int> cellIndex;
    
//     cellIndex.x = (int) std::floor(p.position.x / (double) cellSize.x);
//     cellIndex.y = (int) std::floor(p.position.y / (double) cellSize.y);
//     cellIndex.z = (int) std::floor(p.position.z / (double) cellSize.z);

//     return cellIndex;
// }
