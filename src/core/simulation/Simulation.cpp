
#include "Simulation.h"
#include "DirectSumAlgorithm.h"

/**
 * @brief Factory method to create a simulation instance based on args.
 */
std::unique_ptr<Simulation> Simulation::createSimulation(ParticleContainer &particles, const Args &args) {
    // TODO use args to switch algorithm
    return std::make_unique<DirectSumAlgorithm>(particles, args);
}
