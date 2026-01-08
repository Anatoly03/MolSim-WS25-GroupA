
#include <functional>

#include "../Particle.h"
#include "../utils/Args.h"
#include "../math/Vec3.h"
#include "../Particle.h"

#include "spdlog/spdlog.h"

/**
 * @brief Abstraction for a force calculation lambda between two particles.
 */
using force_calculation_system = std::function<Vec3D(const Args& args, const Particle &a, const Particle &b)>;

/**
 * @details Newton/ Coloumb-like calculation of attraction
 */
inline const force_calculation_system newton_gravity_system = [](const Args & /*args*/, const Particle &par1, const Particle &par2) -> Vec3D {
    Vec3D dist = par2.position - par1.position;

    double r1 = dist.length();
    if (r1 == 0.0) return Vec3D(); // cut in to avoid high values

    double mulMass = par1.mass * par2.mass;
    return dist * (mulMass / (std::pow(r1, 3)));
};

/**
 * @brief Lennard-Jones force calculation system.
 */
inline const force_calculation_system lennard_jones_system = [](const Args &args, const Particle &par1, const Particle &par2) -> Vec3D {
    Vec3D dist = par1.position - par2.position;

    double r1 = dist.length();
    if (r1 > args.cutoff_radius) return Vec3D();  // cut off for performance
    
    // Handle completely overlapping particles (extremely rare with proper initialization)
    // Cannot determine force direction when particles are at exactly the same position
    if (r1 < 1e-10) {
        return Vec3D(0.0, 0.0, 0.0);  // Skip force calculation for this pair
    }

    // Calculate LJ force with actual distance (no artificial clamping)
    // Following physical model strictly, relying on max_force cap to prevent numerical overflow
    double inv_r2 = 1.0 / (r1 * r1);
    double sr2 = std::pow(args.sigma, 2) * inv_r2;
    double sr6 = sr2 * sr2 * sr2;
    double sr12 = sr6 * sr6;

    double scalar = 24.0 * args.epsilon * inv_r2 * (2.0 * sr12 - sr6);
    
    // Cap the force magnitude to prevent numerical instability
    // Very high limit to allow strong LJ repulsion following physical model strictly
    double max_force = 100000.0 * args.epsilon;
    if (std::abs(scalar) > max_force) {
        scalar = (scalar > 0) ? max_force : -max_force;
    }
    
    return scalar * dist.normal();
};

/**
 * @details Returns the force calculation system by its name.
 */
inline const force_calculation_system get_force_system_by_name(const std::string &name) {
    if (name == "newton") return newton_gravity_system;
    if (name == "lennard-jones") return lennard_jones_system;

    spdlog::warn("Force system '{}' not recognized, defaulting to 'lennard-jones'", name);
    return lennard_jones_system; // default
}
