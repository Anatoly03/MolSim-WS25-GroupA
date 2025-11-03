
#include <cmath>
#include <iostream>
#include <list>

#include "FileReader.h"
#include "Frame.h"
#include "ParticleContainer.h"
#include "spdlog/spdlog.h"
#include "utils/ArrayUtils.h"

#ifdef ENABLE_VTK_OUTPUT
#include "outputWriter/VTKWriter.h"
#else
#include "outputWriter/XYZWriter.h"
#endif

/**** forward declaration of the calculation functions ****/

/**
 * calculate the force for all particles
 */
void calculateForce(ParticleContainer& particles);

/**
 * calculate the position for all particles
 */
void calculatePosition(ParticleContainer& particles, double dt);

/**
 * calculate the position for all particles
 */
void calculateVelocity(ParticleContainer& particles, double dt);

/**
 * plot the particles to a xyz-file
 */
void plotParticles(ParticleContainer& particles, int iteration, const char *output_path);

/**
 * @brief The program entry point is the Rahmenprogramm which after getting all
 * variables calls the molecular simulation methods.
 */
int main(int argc, char *argsv[]) {
    const auto args = ProcessArgs(argc, argsv);

    ParticleContainer particles;
    FileReader fileReader;
    fileReader.readFile(particles, args.input_file);
    
    double current_time = args.start_time;
    int iteration = 0;

    // initial plot
    plotParticles(particles, iteration, args.output_path);

    // for this loop, we assume: current x, current f and current v are known
    while (current_time < args.end_time) {
        calculatePosition(particles, args.delta_t);
        calculateForce(particles);
        calculateVelocity(particles, args.delta_t);

        iteration++;
        if (iteration % 10 == 0) {
            plotParticles(particles, iteration, args.output_path);
        }
        spdlog::info("Iteration {} finished.", iteration);

        current_time += args.delta_t;
    }

    spdlog::info("Output written. Terminating...");
    return 0;
}

void calculateForce(ParticleContainer& particles) {
    particles.forEach([&particles](Particle &particle) {
        Vec3D force(0);

        particles.forEach([&particle, &force](const Particle &other) {
            if (other == particle) return;

            Vec3D diffX = other.getPosition() - particle.getPosition();
            double distance = diffX.length();
            double mulMass = particle.getMass() * other.getMass();
            if (distance == 0.0) return;

            force += diffX * (mulMass / (std::pow(distance, 3)));
        });

        particle.delayForce();
        particle.setForce(force);
    });
}

void calculatePosition(ParticleContainer& particles, const double dt) {
    particles.forEach([dt](Particle &particle) {
        Vec3D x = particle.getPosition() + dt * particle.getVelocity() +
                  std::pow(dt, 2) * particle.getForce() / (2 * particle.getMass());
        particle.setPosition(x);
    });
}

void calculateVelocity(ParticleContainer& particles, const double dt) {
    particles.forEach([dt](Particle &particle) {
        Vec3D v =
            particle.getVelocity() + dt * ((particle.getForce() + particle.getOldForce()) / (2 * particle.getMass()));
        particle.setVelocity(v);
    });
}

void plotParticles(ParticleContainer& particles, int iteration, const char *output_path) {
    std::string out_name(output_path);
#ifdef ENABLE_VTK_OUTPUT
    outputWriter::VTKWriter writerVTK;
    writerVTK.plotParticles(particles, out_name, iteration);
#else
    outputWriter::XYZWriter writer;
    writer.plotParticles(particles, out_name, iteration);
#endif
}
