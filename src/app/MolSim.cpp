
#include <cmath>
#include <iostream>
#include <list>

#include "Frame.h"
#include "../core/FileReader.h"
#include "../core/ParticleContainer.h"
#include "../core/utils/ArrayUtils.h"

#ifdef ENABLE_VTK_OUTPUT
#include "../core/writer/VTKWriter.h"
#else
#include "../core/writer/XYZWriter.h"
#endif

/**
 * using ParticleContainer instead of std::list<Particle> particles
 */
static ParticleContainer particles;

/**** forward declaration of the calculation functions ****/

/**
 * calculate the force for all particles
 */
void calculateForce();

/**
 * calculate the position for all particles
 */
void calculatePosition(double dt);

/**
 * calculate the position for all particles
 */
void calculateVelocity(double dt);

/**
 * plot the particles to a xyz-file
 */
void plotParticles(int iteration, const char *output_path);

/**
 * @brief The program entry point is the Rahmenprogramm which after getting all
 * variables calls the molecular simulation methods.
 */
int main(int argc, char *argsv[]) {
    const auto args = ProcessArgs(argc, argsv);

    FileReader fileReader;
    fileReader.readFile(particles, args.input_file);

#ifdef ENABLE_VTK_OUTPUT
    Writer writer = (Writer&)outputWriter::VTKWriter(particles);
#else
    Writer writer = (Writer&)outputWriter::XYZWriter(particles);
#endif

    double current_time = args.start_time;

    int iteration = 0;

    // initial plot
    plotParticles(iteration, args.output_path);

    // for this loop, we assume: current x, current f and current v are known
    while (current_time < args.end_time) {
        calculatePosition(args.delta_t);
        calculateForce();
        calculateVelocity(args.delta_t);

        iteration++;
        if (iteration % 10 == 0) {
            plotParticles(iteration, args.output_path);
        }
        std::cout << "Iteration " << iteration << " finished." << std::endl;

        current_time += args.delta_t;
    }

    std::cout << "output written. Terminating..." << std::endl;
    return 0;
}

void calculateForce() {
    particles.forEach([](Particle &particle) {
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

void calculatePosition(const double dt) {
    particles.forEach([dt](Particle &particle) {
        Vec3D x = particle.getPosition() + dt * particle.getVelocity() +
                  std::pow(dt, 2) * particle.getForce() / (2 * particle.getMass());
        particle.setPosition(x);
    });
}

void calculateVelocity(const double dt) {
    particles.forEach([dt](Particle &particle) {
        Vec3D v =
            particle.getVelocity() + dt * ((particle.getForce() + particle.getOldForce()) / (2 * particle.getMass()));
        particle.setVelocity(v);
    });
}

void plotParticles(int iteration, const char *output_path) {
    std::string out_name(output_path);
    
#ifdef ENABLE_VTK_OUTPUT
    Writer writer = (Writer&)outputWriter::VTKWriter(particles);
#else
    Writer writer = (Writer&)outputWriter::XYZWriter(particles);
#endif
    
    writer.plot(out_name, iteration);
}
