
#include "../math/Vec3.h"
#include "Simulation.h"
#include "DirectSumAlgorithm.h"
#include "LinkedCellImplementation.h"

#include "spdlog/spdlog.h"
#include <cmath>

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
    // Add gravity force if configured
    Vec3D total_force = particle.force + particle.old_force;
    if (arguments.gravity_force.length2() > 0) {
        total_force += particle.mass * arguments.gravity_force * 2.0; // multiply by 2 for averaging
    }
    
    particle.velocity += dt * (total_force / (2 * particle.mass));
}

/**
 * @brief Calculate current kinetic energy of all particles.
 */
double Simulation::calculateKineticEnergy() {
    double kinetic_energy = 0.0;
    int count = 0;
    
    forEachParticle([&](Particle &particle) {
        kinetic_energy += 0.5 * particle.mass * particle.velocity.length2();
        count++;
    });
    
    return kinetic_energy;
}

/**
 * @brief Apply thermostat using velocity rescaling (Maxwell-Boltzmann).
 */
void Simulation::applyThermostat() {
    if (arguments.thermostat_temperature <= 0.0) {
        return; // Thermostat disabled
    }
    
    // Calculate current kinetic energy and temperature
    double current_kinetic_energy = calculateKineticEnergy();
    int particle_count = particleCount();
    
    if (particle_count == 0) {
        return;
    }
    
    // k_B = 1 in reduced units
    // E_kin = (3/2) * N * k_B * T for 3D, (N * k_B * T) for 2D
    // Assuming 3D system
    double current_temperature = (2.0 * current_kinetic_energy) / (3.0 * particle_count);
    
    if (current_temperature <= 0.0) {
        spdlog::warn("Current temperature is zero or negative, skipping thermostat");
        return;
    }
    
    // Calculate scaling factor
    double beta = std::sqrt(arguments.thermostat_temperature / current_temperature);
    
    spdlog::debug("Thermostat: T_current = {}, T_target = {}, beta = {}", 
                  current_temperature, arguments.thermostat_temperature, beta);
    
    // Scale all velocities
    forEachParticle([beta](Particle &particle) {
        particle.velocity *= beta;
    });
}
