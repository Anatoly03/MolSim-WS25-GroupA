
#include <functional>

#include "../Particle.h"
#include "../utils/Args.h"
#include "../utils/TracyHelper.h"
#include "../math/Vec3.h"

#include "spdlog/spdlog.h"

/**
 * @brief Abstraction for a force calculation lambda between two particles.
 */
using force_calculation_system = std::function<Vec3D(const Args& args, const Particle &a, const Particle &b)>;

/**
 * @details Newton/ Coloumb-like calculation of attraction
 */
inline const force_calculation_system newton_gravity_system = [](const Args & /*args*/, const Particle &par1, const Particle &par2) -> Vec3D {
    PROFILE_ZONE_NAMED("Newton Gravity Force Calculation");

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
    PROFILE_ZONE_NAMED("Lennard-Jones Force Calculation");

    Vec3D dist = par1.position - par2.position;

    double r1 = dist.length();
    //std::cout<<"cutoff  "<<args.cutoff_radius<<std::endl;
    if (r1 > args.cutoff_radius) return Vec3D();  // cut off for performance
    double r2 = dist.length2();
    if (r2 == 0.0) return Vec3D(); // cut in to avoid high values

    double averagedSigma = (par1.sigma + par2.sigma) / 2;
    double rootedEpsilon = std::sqrt(par1.epsilon * par2.epsilon);


    double min = (pow(2,1/6)) * averagedSigma;
    //double a = 0. * averagedSigma;
    if (r1 < min) {
        r2 = min * min;
        //r2=r1*r1;
    }
    //r2 = r2 + a * a;

    double inv_r2 = 1.0 / r2;               // (xi -xj)
    //std::cout<<"trigger inv_r2 "<<inv_r2<<std::endl;
    double sr2 = std::pow(averagedSigma, 2) * inv_r2;  // (sigma / (xi -xj))^2
    //std::cout<<"trigger sr2 "<<sr2<<std::endl;

    //std::cout<<"trigger sr2 "<<sr2<<std::endl;
    double sr6 = sr2 * sr2 * sr2;           // (sigma / (xi -xj))^6
    //std::cout<<"trigger sr6 "<<sr6<<std::endl;

    double sr12 = sr6 * sr6;                // (sigma / (xi -xj))^12
    //std::cout<<"trigger sr12 "<<sr12<<std::endl;


    double scalar = 24.0 * rootedEpsilon * inv_r2 * (2.0 * sr12 - sr6);
    //std::cout<<"trigger scalar "<<scalar<<std::endl;

    /*double Fmax = 50.0; // try 50, then adjust up/down
     if (scalar > Fmax) scalar = Fmax;
     if (scalar < -Fmax) scalar = -Fmax;*/

    return scalar * dist.normal();
};

/**
 * @brief Lennard-Jones force calculation system.
 */
inline const force_calculation_system no_force_system = [](const Args &, const Particle &, const Particle &) -> Vec3D {
    return Vec3D();
};

/**
 * @details Returns the force calculation system by its name.
 */
inline const force_calculation_system get_force_system_by_name(const std::string &name) {
    if (name == "newton") return newton_gravity_system;
    if (name == "lennard-jones") return lennard_jones_system;
    if (name== "null") return no_force_system;

    spdlog::warn("Force system '{}' not recognized, defaulting to 'lennard-jones'", name);
    return lennard_jones_system; // default
}
