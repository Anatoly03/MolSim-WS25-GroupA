
#include "../app/Frame.h"
#include "utils/ArrayUtils.h"
#include "ParticleContainer.h"
#include "writer/Writer.h"
#include "Simulation.h"
#include "utils//MaxwellBoltzmannDistribution.h"

#include "spdlog/spdlog.h"

/**
 * @brief calculate the position for all particles
 */
void Simulation::calculatePosition() {
    const double dt = arguments.delta_t;

    particles.forEach([dt](Particle &particle) {
        Vec3D x = particle.getPosition() + dt * particle.getVelocity() +
                  std::pow(dt, 2) * particle.getForce() / (2 * particle.getMass());
        particle.setPosition(x);
    });
}

/**
 * @brief calculate the velocity for all particles
 */
void Simulation::calculateVelocity() {
    const double dt = arguments.delta_t;

    particles.forEach([dt](Particle &particle) {
        Vec3D v =
            particle.getVelocity() + dt * ((particle.getForce() + particle.getOldForce()) / (2 * particle.getMass()));
        particle.setVelocity(v);
    });
}

/**
 * @brief delay the force for all particles
 */
void Simulation::delayForce() {
    particles.forEach([](Particle &particle) {
        particle.delayForce();
    });
}

/**
 * @brief calculate the force for all particles, asserts 
 * @note naive O(n^2) implementation
 * @todo replace with efficient algorithm
 */
void Simulation::calculateForce() {
    // TODO if in debug mode, assert that all forces are zero
    // #if TODO
    // particles.forEach([](Particle &particle) {
    //     assert(particle.getForce().length() == 0 && "Forces must be zero before force calculation.");
    // });
    // #endif

    particles.forEachDistinctPair([this](Particle &particle, Particle &other) {
        Vec3D diffX = other.getPosition() - particle.getPosition();
        double distance = diffX.length();
        double mulMass = particle.getMass() * other.getMass();
        if (distance == 0.0) return;

        Vec3D force = diffX * (mulMass / (std::pow(distance, 3)));

        particle.addForce(force);
        other.addForce(-force);
    });



}
ParticleContainer particleGenerator(Vec3<double> firstCorrdinate ,int N_1,int N_2, int N_3, int h,int mass,Vec3<double> initialVelocity){

    ParticleContainer container;
    container.reserve(N_1*N_2*N_3);
    for (int i = 0; i < N_1; ++i) {
        for (int j = 0; j <N_2; ++j) {
            for (int k = 0; k < N_3; ++k) {
                Vec3D position = firstCorrdinate + Vec3D(i * h, j * h, k * h);
                //what should be the average?
                Vec3D randomVelo=maxwellBoltzmannDistributedVelocity(1.0, 3);
                Vec3D velocity = initialVelocity + randomVelo;
                container.emplace_back(position,velocity,mass);
            }

        }

    }

}