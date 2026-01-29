
#pragma once

#include <functional>
#include <cmath>

#include "../Particle.h"
#include "../utils/Args.h"
#include "../math/Vec3.h"
#include "spdlog/spdlog.h"


/**
 * @brief Abstraction for a force calculation lambda between two particles.
 */
using force_calculation_system = std::function<Vec3D(const Args& args, const Particle &a, const Particle &b)>;

inline double smoothingS(double d2, double rl, double rc) {
    const double rl2 = rl * rl;
    const double rc2 = rc * rc;
    if (d2 <= rl2) return 1.0;
    if (d2 >= rc2) return 0.0;

    const double denom = rc2 - rl2;
    const double a = d2 - rl2;
    const double numerator = (a * a) * (3.0 * rc2 - rl2 - 2.0 * d2);
    return 1.0 - numerator / (denom * denom * denom);
}

inline double smoothedLJPotential(double d, double epsilon, double sigma, double rl, double rc) {
    if (d <= 1e-12) return 0.0;
    const double d2 = d * d;
    const double rc2 = rc * rc;
    if (d2 >= rc2) return 0.0;
    const double S = smoothingS(d2, rl, rc);

    const double inv = sigma / d;
    const double inv2 = inv * inv;
    const double inv6 = inv2 * inv2 * inv2;
    const double inv12 = inv6 * inv6;
    return 4.0 * epsilon * S * (inv12 - inv6);
}

inline double smoothingS_derivative(double d2, double rl, double rc) {
    const double rl2 = rl * rl;
    const double rc2 = rc * rc;
    if (d2 <= rl2 || d2 >= rc2) return 0.0;

    const double denom = rc2 - rl2;
    const double a = d2 - rl2;
    const double denom_cubed = denom * denom * denom;
    const double df = 2.0 * a * (3.0 * rc2 - rl2 - 2.0 * d2) - 2.0 * a * a;
    return -df / denom_cubed;
}

inline Vec3D ljForceVector(const Vec3D& r, double d, double epsilon, double sigma) {
    const double s2 = sigma * sigma;
    const double d2 = d * d;
    const double inv_d2 = 1.0 / d2;
    const double inv_d6 = inv_d2 * inv_d2 * inv_d2;
    const double sig6 = s2 * s2 * s2;
    const double sig12 = sig6 * sig6;
    const double inv_d8 = inv_d6 * inv_d2;
    const double inv_d14 = inv_d6 * inv_d6 * inv_d2;
    const double factor = 24.0 * epsilon * (2.0 * sig12 * inv_d14 - sig6 * inv_d8);
    return r * factor;
}

inline Vec3D smoothedLJForce(const Vec3D& xi, const Vec3D& xj, double epsilon, 
                             double sigma, double rl, double rc) {
    const Vec3D r = xi - xj; // xj -> xi
    const double d2 = r.length2();
    const double rl2 = rl * rl;
    const double rc2 = rc * rc;
    if (d2 <= 1e-24) return Vec3D();
    if (d2 >= rc2) return Vec3D();

    const double d = std::sqrt(d2);
    if (d <= rl) {
        return ljForceVector(r, d, epsilon, sigma);
    }

    const double S = smoothingS(d2, rl, rc);
    const double dS_dd2 = smoothingS_derivative(d2, rl, rc);
    const double dS_dr = dS_dd2 * 2.0 * d;

    const double sig2 = sigma * sigma;
    const double sig6 = sig2 * sig2 * sig2;
    const double sig12 = sig6 * sig6;

    const double inv_r = 1.0 / d;
    const double inv_r2 = inv_r * inv_r;
    const double inv_r6 = inv_r2 * inv_r2 * inv_r2;
    const double inv_r8 = inv_r6 * inv_r2;
    const double inv_r12 = inv_r6 * inv_r6;
    const double inv_r14 = inv_r12 * inv_r2;

    const double A = sig12 * inv_r12 - sig6 * inv_r6;
    const double scalar = 48.0 * epsilon * S * sig12 * inv_r14
                        - 24.0 * epsilon * S * sig6 * inv_r8
                        - 4.0 * epsilon * A * dS_dr * inv_r;

    return r * scalar;
}

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
    //std::cout<<"cutoff  "<<args.cutoff_radius<<std::endl;
    if (r1 > args.cutoff_radius) return Vec3D();  // cut off for performance
    if (r1 == 0.0) return Vec3D(); // cut in to avoid high values

    double averagedSigma = (par1.sigma + par2.sigma) / 2;
    double rootedEpsilon = std::sqrt(par1.epsilon * par2.epsilon);


    double min = (pow(2,1/6)) * averagedSigma;
    if (r1 < min) {
        r1 = min;
    }

    return ljForceVector(dist, r1, rootedEpsilon, averagedSigma);
};

inline const force_calculation_system smoothed_lennard_jones_system = [](const Args &args, const Particle &par1, const Particle &par2) -> Vec3D {
    // Smoothed Lennard-Jones force calculation
    double averagedSigma = (par1.sigma + par2.sigma) / 2.0;
    double rootedEpsilon = std::sqrt(par1.epsilon * par2.epsilon);

    return smoothedLJForce(par1.position, par2.position, rootedEpsilon, 
                          averagedSigma, args.smoothing_radius_lower, args.cutoff_radius);
};

/**
 * @details Returns the force calculation system by its name.
 */
inline const force_calculation_system get_force_system_by_name(const std::string &name) {
    if (name == "newton") return newton_gravity_system;
    if (name == "lennard-jones") return lennard_jones_system;
    if (name == "smoothed-lennard-jones") return smoothed_lennard_jones_system;

    spdlog::warn("Force system '{}' not recognized, defaulting to 'lennard-jones'", name);
    return lennard_jones_system;
}
