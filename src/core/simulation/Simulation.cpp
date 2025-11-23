
#include "Simulation.h"
#include "DirectSumAlgorithm.h"
#include "LinkedCellImplementation.h"

#include "spdlog/spdlog.h"

/**
 * @brief Factory method to create a simulation instance based on args.
 */
std::unique_ptr<Simulation> Simulation::createSimulation(ParticleContainer &particles, const Args &args) {
    switch (args.version) {
        case 0:
            return std::make_unique<DirectSumAlgorithm>(particles, args);
        case 1:
            return std::make_unique<LinkedCellImplementation>(particles, args,1,5,5,5,2.5);
    }

    spdlog::critical("unreachable: args.version was {} in simulation factory", args.version);
    return std::make_unique<DirectSumAlgorithm>(particles, args);
}
