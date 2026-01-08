
#include <functional>

#include "../Particle.h"
#include "../math/Vec3.h"
#include "../simulation/Simulation.h"

// #include "spdlog/spdlog.h"

// /**
//  * @brief Abstraction for a force calculation lambda between two particles.
//  * @returns True, if the parent should destroy the particle.
//  */
// using boundary_condition_system = std::function<bool(const Simulation& sim, Particle &p, Vec3I ghostCellIndex)>;

// /**
//  * @brief Outflow boundary condition system.
//  */
// inline const boundary_condition_system outflow_boundary_system = [](const Simulation & /*sim*/, Particle &p, Vec3I ghostCellIndex) {
//     // TODO compare previous and current position, check for collision at border.
    
//     // always remove the particle when it hits the boundary
//     return true;
// };

// /**
//  * @details Returns the force calculation system by its name.
//  */
// inline const boundary_condition_system get_boundary_condition_by_name(const std::string &name) {
//     if (name == "reflect") return ...;
//     if (name == "outflow") return ...;

//     spdlog::warn("Boundary condition '{}' not recognized, defaulting to '...'", name);
//     return ...; // default
// }
