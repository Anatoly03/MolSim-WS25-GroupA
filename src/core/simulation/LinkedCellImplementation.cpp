
#include "../math/Vec3.h"
#include "../utils/Args.h"
#include "../LinkedCells.h"
#include "../ParticleContainer.h"
#include "LinkedCellImplementation.h"

/**
 * @brief Reindex particles that switched cells.
 */
void LinkedCellImplementation::reindexParticles() {
    cells.reindex();
    
    // Apply hard boundary constraints for reflective boundaries
    // This prevents particles from escaping when forces are insufficient
    const Vec3D domainMin = Vec3D(cells.domainMin);
    const Vec3D domainMax = Vec3D(cells.domainMax);
    
    cells.forEach([&](Particle &p) {
        // Clamp position and reflect velocity for reflective boundaries
        if (cells.boarderXmin == "reflect" && p.position.x < domainMin.x) {
            p.position.x = domainMin.x;
            p.velocity.x = std::abs(p.velocity.x);
        }
        if (cells.boarderXmax == "reflect" && p.position.x > domainMax.x) {
            p.position.x = domainMax.x;
            p.velocity.x = -std::abs(p.velocity.x);
        }
        if (cells.boarderYmin == "reflect" && p.position.y < domainMin.y) {
            p.position.y = domainMin.y;
            p.velocity.y = std::abs(p.velocity.y);
        }
        if (cells.boarderYmax == "reflect" && p.position.y > domainMax.y) {
            p.position.y = domainMax.y;
            p.velocity.y = -std::abs(p.velocity.y);
        }
        if (cells.boarderZmin == "reflect" && p.position.z < domainMin.z) {
            p.position.z = domainMin.z;
            p.velocity.z = std::abs(p.velocity.z);
        }
        if (cells.boarderZmax == "reflect" && p.position.z > domainMax.z) {
            p.position.z = domainMax.z;
            p.velocity.z = -std::abs(p.velocity.z);
        }
    });
    
    // Hard-core repulsion: prevent particles from getting too close
    // When distance < min_distance, directly push particles apart AND correct velocities
    const double min_distance = 1.2;  // Hard minimum distance (equal to sigma)
    const double min_dist_sq = min_distance * min_distance;
    
    cells.forEachDistinctPair([&](Particle &p1, Particle &p2) {
        Vec3D delta = p1.position - p2.position;
        double dist_sq = delta.length2();
        
        // If particles are too close, push them apart and correct velocities
        if (dist_sq < min_dist_sq && dist_sq > 1e-10) {
            double dist = std::sqrt(dist_sq);
            double overlap = min_distance - dist;
            
            // Separation direction (from p2 to p1)
            Vec3D separation_dir = delta.normal();
            
            // Push each particle half the overlap distance along separation vector
            Vec3D position_correction = separation_dir * (overlap * 0.5);
            p1.position += position_correction;
            p2.position -= position_correction;
            
            // Velocity correction: remove relative velocity component that brings particles closer
            // Project relative velocity onto separation direction
            Vec3D relative_vel = p1.velocity - p2.velocity;
            double vel_along_separation = relative_vel.dot(separation_dir);
            
            // If particles are approaching each other, zero out the approaching component
            if (vel_along_separation < 0.0) {
                Vec3D vel_correction = separation_dir * (vel_along_separation * 0.5);
                p1.velocity -= vel_correction;  // Reduce p1's velocity toward p2
                p2.velocity += vel_correction;  // Reduce p2's velocity toward p1
            }
        }
    });
};

/**
 * @brief calculate special physics for domain-bordering cells, e.g.
 * particle-border collision or particle extinction
 */
void LinkedCellImplementation::calculateBorderBehaviour() {
    // brief: simple repulsion from border using ghost particles
    //const int REPULSION = 1;



    const double dist = std::pow(2.0, 1.0/6.0) * arguments.sigma;

    cells.forEachBordered([&](Particle &p, Vec3I /*ghostCellIndex*/) {


        //
        //     cell    ghost cell
        // |----------| - - - - -|
        // |          |          |
        // |  x------ | ------x' |
        // |          |          |
        // |----------| - - - - -|
        //   cellSize -> mirror!
        //    |----------------------| twice cell
        //                     |-----| minus twice relative
        //

        // TODO reflecting boundary condition / verify math
        // TODO are particles attracted to border?


        // X min wall
        if(cells.boarderXmin=="reflect") {
            double dxMin = p.position.x - domainMin.x;
            if (dxMin < dist) {
                Particle wall;
                wall.position = Vec3D(domainMin.x, p.position.y, p.position.z);
                auto f = forceCalculationSystem(const_cast<Args &>(arguments), p, wall);
                p.force += f;
            }
        }

        // X max wall
        //std::cout<<boxMax.x<<std::endl;
        if(cells.boarderXmax=="reflect") {
            double dxMax = domainMax.x - p.position.x;
            if (dxMax < dist) {
                Particle wall;
                wall.position = Vec3D(domainMax.x, p.position.y, p.position.z);
                auto f = forceCalculationSystem(const_cast<Args &>(arguments), p, wall);
                p.force += f;
            }
        }

        // Y min wall
        if(cells.boarderYmin=="reflect") {
            double dyMin = p.position.y - domainMin.y;
            if (dyMin < dist) {
                Particle wall;
                wall.position = Vec3D(p.position.x, domainMin.y, p.position.z);
                auto f = forceCalculationSystem(const_cast<Args &>(arguments), p, wall);
                p.force += f;
            }
        }

        // Y max wall
        if(cells.boarderYmax=="reflect") {
            double dyMax = domainMax.y - p.position.y;
            if (dyMax < dist) {
                Particle wall;
                wall.position = Vec3D(p.position.x, domainMax.y, p.position.z);
                auto f = forceCalculationSystem(const_cast<Args &>(arguments), p, wall);
                p.force += f;
            }
        }

        // Z min wall
        if(cells.boarderZmin=="reflect") {
            double dzMin = p.position.z - domainMin.z;
            if (dzMin < dist) {
                Particle wall;
                wall.position = Vec3D(p.position.x, p.position.y, domainMin.z);
                auto f = forceCalculationSystem(const_cast<Args &>(arguments), p, wall);
                p.force += f;
            }
        }

        // Z max wall
        if(cells.boarderZmax=="reflect") {
            double dzMax = domainMax.z - p.position.z;
            if (dzMax < dist) {
                Particle wall;
                wall.position = Vec3D(p.position.x, p.position.y, domainMax.z);
                auto f = forceCalculationSystem(const_cast<Args &>(arguments), p, wall);
                p.force += f;
            }
        }
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

//         double r_len = r.length2();
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
