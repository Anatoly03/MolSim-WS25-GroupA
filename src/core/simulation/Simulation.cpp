
#include "../math/Vec3.h"
#include "Simulation.h"
#include "DirectSumAlgorithm.h"
#include "LinkedCellImplementation.h"

#include "spdlog/spdlog.h"
#include <fmt/format.h>

/**
 * @brief Factory method to create a simulation instance based on args.
 */
std::shared_ptr<Simulation> Simulation::createSimulation(ParticleContainer &particles, const Args &args) {
    switch (args.version) {
        case 0:
            return std::make_shared<DirectSumAlgorithm>(particles, args);
        case 1:
            return std::make_shared<LinkedCellImplementation>(particles, args);
    }

    spdlog::critical("unreachable: args.version was {} in simulation factory", args.version);
    return std::make_shared<DirectSumAlgorithm>(particles, args);
}

// /**
//  * @brief Calculates the Lennard-Jones potential between two particles.
//  * 
//  * @details This function implements a pairwise potential computation using the Lennard-Jones potential,
//  * which are applied symmetrically to both particles in the pair, following Newton’s third law.
//  */
// double Simulation::calculateLennardJonesPotential(Particle& p_i, Particle& p_j, double epsilon, double sigma) {
//     const double MAX_FORCE = 100.0;
//     const double CUT_EXACT_EQUALITY = 1e-12;
//     const double CUT_OFF = 4.0;

//     const double p_delta = (p_i.position - p_j.position).length2();

//     // avoid exact-equality and division by zero
//     if (p_delta <= CUT_EXACT_EQUALITY) { return MAX_FORCE; }
//     // cut off radius
//     if (p_delta >= CUT_OFF) { return 0.0; }

//     const auto factor = sigma / p_delta;
//     const double sig6 = std::pow(factor, 6);
//     const double sig12 = std::pow(factor, 12);
//     double x = 4 * epsilon * (sig12 - sig6);

//     return 4 * epsilon * (sig12 - sig6);
// }

/**
 * @brief update the position for a single particles
 */
void Simulation::calculateSinglePosition(Particle &particle, double dt) {
    particle.position += dt * particle.velocity + std::pow(dt, 2) * particle.force / (2 * particle.mass);
    /*Vec3D x = dt * particle.velocity + std::pow(dt, 2) * particle.force / (2 * particle.mass);
    if ( particle.position.x + x.x > std::numeric_limits<double>::max()) {
        particle.position.x = std::numeric_limits<double>::max();
    }else{
        particle.position.x = particle.velocity.x + x.x;

    }
    if ( particle.position.y + x.y > std::numeric_limits<double>::max()) {
        particle.position.y = std::numeric_limits<double>::max();
    }else{
        particle.position.y = particle.velocity.y + x.y;

    }
    if ( particle.position.z + x.z > std::numeric_limits<double>::max()) {
        particle.position.z = std::numeric_limits<double>::max();
    }else{
        particle.position.z = particle.velocity.z + x.z;

    }*/
}

/**
 * @brief update the velocity for a single particles
 */
void Simulation::calculateSingleVelocity(Particle &particle, double dt) {
    particle.velocity += dt * ((particle.force + particle.old_force) / (2 * particle.mass));

    // Vec3D x = dt * ((particle.force + particle.old_force) / (2 * particle.mass));
    // if ( particle.velocity.x + x.x > std::numeric_limits<double>::max()) {
    //     particle.velocity.x = std::numeric_limits<double>::max();
    // }else if ( particle.velocity.x + x.x < std::numeric_limits<double>::min()) {
    //     particle.velocity.x = std::numeric_limits<double>::min();
    // }else{
    //     particle.velocity.x = particle.velocity.x + x.x;

    // }
    // if ( particle.velocity.y +x.y > std::numeric_limits<double>::max()) {

    //     particle.velocity.y = std::numeric_limits<double>::max();

    // }else if ( particle.velocity.y + x.y < std::numeric_limits<double>::min()) {

    //     particle.velocity.y = std::numeric_limits<double>::min();
    // }else{
    //     particle.velocity.y = particle.velocity.y + x.y;

    // }
    // if ( particle.velocity.z + x.z > std::numeric_limits<double>::max()) {
    //     particle.velocity.z = std::numeric_limits<double>::max();
    // }else if ( particle.velocity.z + x.z < std::numeric_limits<double>::min()) {
    //     particle.velocity.z = std::numeric_limits<double>::min();
    // }else{
    //     particle.velocity.z = particle.velocity.z + x.z;
    // }
}

/**
 * @brief Calculates force acting on two particles.
 */
void Simulation::calculateSingleForce(Particle& p_i, Particle& p_j) {
    Vec3D dist = p_i.position - p_j.position;

    double r1 = dist.length();
    if (r1 > arguments.cutoff_radius) return;  // cut off for performance

    double r2 = dist.length2();
    if (r2 == 0.0) return; // cut in to avoid high values

    double min=pow(2,1/6)*arguments.sigma;
    if (r2 < min) r2=min;

    double inv_r2 = 1.0 / r2;               // (xi -xj)
    double sr2 = std::pow(arguments.sigma, 2) * inv_r2;  // (sigma / (xi -xj))^2
    double sr6 = sr2 * sr2 * sr2;           // (sigma / (xi -xj))^6
    double sr12 = sr6 * sr6;                // (sigma / (xi -xj))^12

    double scalar = 24.0 * arguments.epsilon * inv_r2 * (2.0 * sr12 - sr6);

    Vec3D force = scalar * dist.normal();

    spdlog::trace("Particles {}-{} exert force {}x{}", p_i.p_id, p_j.p_id, scalar, dist.normal());

    p_i.force += force;
    p_j.force -= force;
}
