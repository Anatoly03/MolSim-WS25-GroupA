
#include "../math/Vec3.h"
#include "Simulation.h"
#include "LinkedCellImplementation.h"

#include "spdlog/spdlog.h"

/**
 * @brief Factory method to create a simulation instance based on args.
 */
std::shared_ptr<Simulation> Simulation::createSimulation(ParticleContainer &particles, const Args &args) {
    switch (args.version) {
        case 0:
            return std::make_shared<Simulation>(particles, args);
        case 1:
            return std::make_shared<LinkedCellImplementation>(particles, args);
    }

    spdlog::critical("unreachable: args.version was {} in simulation factory", args.version);
    return std::make_shared<Simulation>(particles, args);
}

/**
 * @brief update the position for a single particles
 */
void Simulation::calculateSinglePosition(Particle &particle, double dt) {
    particle.position += dt * particle.velocity + std::pow(dt, 2) * particle.force / (2 * particle.mass);
}

/**
 * @brief update the velocity for a single particles
 */
void Simulation::calculateSingleVelocity(Particle &particle, double dt) {
    particle.velocity += dt * ((particle.force + particle.old_force) / (2 * particle.mass));

}
