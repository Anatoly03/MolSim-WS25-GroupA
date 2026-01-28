
#pragma once

#include <functional>

#include "../Particle.h"
#include "../utils/Args.h"
#include "../math/Vec3.h"
#include "spdlog/spdlog.h"


/**
 * @brief Abstraction for a force calculation lambda between two particles.
 */
using force_calculation_system = std::function<Vec3D(const Args& args, const Particle &a, const Particle &b)>;

inline double smoothingS(double d, double rl, double rc) {
    if (d <= rl) return 1.0;
    if (d >= rc) return 0.0;

    const double denom = rc - rl;
    const double a = d - rl;
    const double numerator = (a * a) * (3.0 * rc - rl - 2.0 * d);
    return 1.0 - numerator / (denom * denom * denom);
}

inline double smoothedLJPotential(double d, double epsilon, double sigma, double rl, double rc) {
    if (d <= 1e-12) return 0.0;
    if (d >= rc) return 0.0;
    const double S = smoothingS(d, rl, rc);

    const double inv = sigma / d;
    const double inv2 = inv * inv;
    const double inv6 = inv2 * inv2 * inv2;
    const double inv12 = inv6 * inv6;
    return 4.0 * epsilon * S * (inv12 - inv6);
}

inline double smoothingS_derivative(double d, double rl, double rc) {
    if (d <= rl || d >= rc) return 0.0;

    const double denom = rc - rl;
    const double a = d - rl;
    const double denom_cubed = denom * denom * denom;
    return -6.0 * a * (rc - d) / denom_cubed;
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

inline double smoothedLJForceScalarMid(double d, double rl, double rc, 
                                       double epsilon, double sigma) {
    const double sig2 = sigma * sigma;
    const double sig6 = sig2 * sig2 * sig2;
    const double d2 = d * d;
    const double d3 = d2 * d;
    const double d6 = d3 * d3;
    const double d7 = d6 * d;
    const double d14 = d7 * d7;
    const double denom = rc - rl;
    const double denom3 = denom * denom * denom;
    const double rc2 = rc * rc;
    
    const double term1 = rc2 * (2.0 * sig6 - d6);
    const double term2 = rc * (3.0 * rl - d) * (d6 - 2.0 * sig6);
    const double term3 = d * (5.0 * rl * sig6 - 2.0 * rl * d6 - 3.0 * sig6 * d + d7);
    const double big = (rc - d) * (term1 + term2 + term3);
    const double pref = -24.0 * sig6 * epsilon / (d14 * denom3);
    
    return pref * big;
}

inline Vec3D smoothedLJForce(const Vec3D& xi, const Vec3D& xj, double epsilon, 
                             double sigma, double rl, double rc) {
    const Vec3D r = xj - xi;
    const double d = r.length();
    if (d <= 1e-12) return Vec3D();
    if (d >= rc) return Vec3D();
    if (d <= rl) return ljForceVector(r, d, epsilon, sigma);
    const double scalar = smoothedLJForceScalarMid(d, rl, rc, epsilon, sigma);
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
