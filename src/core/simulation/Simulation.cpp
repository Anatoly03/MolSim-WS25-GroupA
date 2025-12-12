
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
 * @brief update the position for a single particles
 */
void Simulation::calculateSinglePosition(Particle &particle, double dt) {
    particle.position += dt * particle.velocity + std::pow(dt, 2) * particle.force / (2 * particle.mass);
}

/**
 * @brief update the velocity for a single particles
 */
void Simulation::calculateSingleVelocity(Particle &particle, double dt) {
    if(iteration % arguments.thermostatStep == 0) {
        if (arguments.temperatureScaling == "gradual") {
            if (std::abs(arguments.temperature - currentTemperature) > arguments.maximumTemperatureDifference) {

                if (arguments.temperature > currentTemperature) {
                    double beta = std::sqrt((this->currentTemperature + arguments.maximumTemperatureDifference) /
                                            this->currentTemperature);
                    particle.velocity += (dt * ((particle.force + particle.old_force) / (2 * particle.mass))) * beta;
                } else {
                    double beta = std::sqrt((this->currentTemperature - arguments.maximumTemperatureDifference) /
                                            this->currentTemperature);
                    particle.velocity += (dt * ((particle.force + particle.old_force) / (2 * particle.mass))) * beta;
                }


            }
        } else if (arguments.temperatureScaling == "directly") {
            double beta = std::sqrt(arguments.temperature / this->currentTemperature);
            particle.velocity += (dt * ((particle.force + particle.old_force) / (2 * particle.mass))) * beta;
        } else {
            particle.velocity += (dt * ((particle.force + particle.old_force) / (2 * particle.mass)));

        }
    }else{
        particle.velocity += (dt * ((particle.force + particle.old_force) / (2 * particle.mass)));
    }

}


