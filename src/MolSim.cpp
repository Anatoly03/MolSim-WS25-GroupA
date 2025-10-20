
#include <iostream>
#include <list>

#include "Frame.h"
#include "FileReader.h"
#include "outputWriter/XYZWriter.h"
#include "utils/ArrayUtils.h"

/**** forward declaration of the calculation functions ****/

/**
 * calculate the force for all particles
 */
void calculateF();

/**
 * calculate the position for all particles
 */
void calculateX(double dt);

/**
 * calculate the position for all particles
 */
void calculateV(double dt);

/**
 * plot the particles to a xyz-file
 */
void plotParticles(int iteration);

// TODO: what data structure to pick?
std::list<Particle> particles;

/**
 * The program entry point is the Rahmenprogramm which after getting all variables
 * calls the molecular simulation methods.
 */
int main(int argc, char *argsv[]) {
  const auto args = ProcessArgs(argc, argsv);

  FileReader fileReader;
  fileReader.readFile(particles, args.input_file);

  double current_time = args.start_time;

  int iteration = 0;

  // for this loop, we assume: current x, current f and current v are known
  while (current_time < args.end_time) {
    // calculate new x
    calculateX(args.delta_t);
    // calculate new f
    calculateF();
    // calculate new v
    calculateV(args.delta_t);

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

void calculateF() {
    std::list<Particle>::iterator iterator;
    iterator = particles.begin();

    for (auto &p1 : particles) {
        Vec3D force(0);

        for (auto &p2 : particles) {
            if (p1 == p2) continue;

            Vec3D diffX = p2.getX() - p1.getX();
            double distance = diffX.length();
            double mulMass = p1.getM() * p2.getM();

            force += diffX * (mulMass / (std::pow(distance, 3)));
        }

        p1.delayF();
        p1.setF(force);
    }
}

void calculateX(double dt) {
    for (auto &p : particles) {
        Vec3D x = p.getX() + dt * p.getV() + std::pow(dt, 2) * p.getF() / (2 * p.getM());
        p.setX(x);
    }
}

void calculateV(double dt) {
    for (auto &p : particles) {
        Vec3D v = p.getV() + dt * ((p.getF() + p.getOldF()) / (2 * p.getM()));
        p.setV(v);
    }
}

void plotParticles(int iteration) {
  std::string out_name("MD_vtk");

  outputWriter::XYZWriter writer;
  writer.plotParticles(particles, out_name, iteration);
}
