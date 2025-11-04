
#include <cmath>
#include <iostream>
#include <list>

#include "Frame.h"
#include "../core/FileReader.h"
#include "../core/ParticleContainer.h"
#include "../core/Simulation.h"

#ifdef ENABLE_VTK_OUTPUT
#include "../core/writer/VTKWriter.h"
#else
#include "../core/writer/XYZWriter.h"
#endif

/**
 * @brief The program entry point is the Rahmenprogramm which after getting all
 * variables calls the molecular simulation methods.
 */
int main(int argc, char *argsv[]) {
    const auto args = ProcessArgs(argc, argsv);

    ParticleContainer particles;
    Simulation simulation(particles, args);

    // set up file IO
    FileReader fileReader;
    fileReader.readFile(particles, args.input_file);

#ifdef ENABLE_VTK_OUTPUT
    Writer writer = outputWriter::VTKWriter(particles);
#else
    Writer writer = outputWriter::XYZWriter(particles);
#endif
    simulation.setWriter(writer);

    // everything ready - run the simulation
    simulation.run();

    return 0;
}
