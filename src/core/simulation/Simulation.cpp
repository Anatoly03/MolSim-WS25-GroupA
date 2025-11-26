
#include "../math/Vec3.h"
#include "Simulation.h"
#include "DirectSumAlgorithm.h"
#include "LinkedCellImplementation.h"

#include "spdlog/spdlog.h"

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

/**
 * @brief Calculates the Lennard-Jones potential between two particles.
 * 
 * @details This function implements a pairwise potential computation using the Lennard-Jones potential,
 * which are applied symmetrically to both particles in the pair, following Newton’s third law.
 */
double Simulation::calculateLennardJonesPotential(Particle& p_i, Particle& p_j, double epsilon, double sigma) {
    const double MAX_FORCE = 100.0;
    const double CUT_EXACT_EQUALITY = 1e-12;
    const double CUT_OFF = 4.0;

    const double p_delta = (p_i.position - p_j.position).length();

    // avoid exact-equality and division by zero
    if (p_delta <= CUT_EXACT_EQUALITY) { return MAX_FORCE; }
    // cut off radius
    if (p_delta >= CUT_OFF) { return 0.0; }

    const auto factor = sigma / p_delta;
    const double sig6 = std::pow(factor, 6);
    const double sig12 = std::pow(factor, 12);

    return 4 * epsilon * (sig12 - sig6);
}

/**
 * @brief Calculates force acting on two particles.
 */
void Simulation::calculateSingleForce(Particle& p_i, Particle& p_j) {
    // TODO lennard-Jones Parameters (hard code for now)
    const double epsilon = 5.0;
    const double sigma = 1.0;

    Vec3D r = p_i.position - p_j.position;
    double r_len = r.length();
    if (r_len == 0.0) { return; }

    // rf. formula (3)
    double inv_r2 = 1.0 / (r_len * r_len);
    double sigma_r2 = sigma * inv_r2;
    double scalar = -24.0 * epsilon * std::pow(inv_r2, 2) * (std::pow(sigma_r2, 6) - 2.0 * std::pow(sigma_r2, 12));
    Vec3D F_ij = scalar * r;

    p_i.force += F_ij;
    p_j.force -= F_ij;
}
