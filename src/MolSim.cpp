
#include <cmath>
#include <iostream>
#include <list>

#include "FileReader.h"
#include "Frame.h"
#include "ParticleContainer.h"
#include "utils/ArrayUtils.h"

#ifdef ENABLE_VTK_OUTPUT
#include "outputWriter/VTKWriter.h"
#else
#include "outputWriter/XYZWriter.h"
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
void plotParticles(int iteration);

/**
 * copy the list from FileReader into ParticleConatiner
 */
static void loadParticleContainer(std::list<Particle> &src, ParticleContainer &dst);

/**
 * @brief The program entry point is the Rahmenprogramm which after getting all
 * variables calls the molecular simulation methods.
 */
int main(int argc, char *argsv[]) {
    const auto args = ProcessArgs(argc, argsv);

    FileReader fileReader;
    fileReader.readFile(particles, args.input_file);

    double current_time = args.start_time;

    int iteration = 0;

    // for this loop, we assume: current x, current f and current v are known
    while (current_time < args.end_time) {
        calculatePosition(args.delta_t);
        calculateForce();
        calculateVelocity(args.delta_t);

        iteration++;
        if (iteration % 10 == 0) {
            plotParticles(iteration);
        }
        std::cout << "Iteration " << iteration << " finished." << std::endl;

        current_time += args.delta_t;
    }

    std::cout << "output written. Terminating..." << std::endl;
    return 0;
}

void calculateForce() {
    // save old force and initialization
    particles.forEach([](Particle &particle) { particle.delayForce(); });

    // accumulate as pair
    particles.forEachPair([](Particle &a, Particle &b) {
        Vec3D diffX = b.getPosition() - a.getPosition();
        double distance = diffX.length();
        if (distance == 0.0) return;
        double mulMass = a.getMass() * b.getMass();

        Vec3D force = diffX * mulMass / pow(distance, 3);

        auto forceA = a.getForce();
        forceA += force;
        a.setForce(forceA);

        auto forceB = b.getForce();
        forceB -= force;
        b.setForce(forceB);
    });
}

void calculatePosition(double dt) {
    for (auto &p : particles) {
        Vec3D x = p.getPosition() + dt * p.getVelocity() + std::pow(dt, 2) * p.getForce() / (2 * p.getMass());
        p.setPosition(x);
    }
}

void calculateVelocity(double dt) {
    for (auto &p : particles) {
        Vec3D v = p.getVelocity() + dt * ((p.getForce() + p.getOldForce()) / (2 * p.getMass()));
        p.setVelocity(v);
    }
}

void plotParticles(int iteration) {
    std::string out_name("MD_vtk");
#ifdef ENABLE_VTK_OUTPUT
    outputWriter::VTKWriter writerVTK;
    writerVTK.plotParticles(particles, out_name, iteration);
#else
    outputWriter::XYZWriter writer;
    writer.plotParticles(particles, out_name, iteration);
#endif
}
