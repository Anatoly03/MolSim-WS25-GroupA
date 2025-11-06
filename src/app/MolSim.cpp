#include <time.h>

#include "../core/FileReader.h"
#include "../core/ParticleContainer.h"
#include "../core/Simulation.h"
#include "Frame.h"

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

    // set up file IO
    FileReader fileReader;
    fileReader.readFile(particles, args.input_file);

    // set up simulation
    if (!args.benchmark_enabled) {
        Simulation simulation(particles, args);

#ifdef ENABLE_VTK_OUTPUT
        outputWriter::VTKWriter writer(particles);
#else
        outputWriter::XYZWriter writer(particles);
#endif
        simulation.setWriter(writer);

        // everything ready - run the simulation
        simulation.run();
        return 0;
    }

    // set up benchmarking
    const int bits = args.benchmark_iterations;
    
    timespec starttime;
    timespec end;
    double total_duration = 0.0;

    for (int i = 0; i < bits; i++) {
        ParticleContainer copy(particles);
        Simulation simulation(copy, args);

        clock_gettime(CLOCK_MONOTONIC, &starttime);

        simulation.run();

        clock_gettime(CLOCK_MONOTONIC, &end);

        double duration = (double)(end.tv_sec - starttime.tv_sec) + ((double)(end.tv_nsec - starttime.tv_nsec) * 1e-9);
        total_duration += duration;
        std::cout << "Benchmark Iteration " << i << ": " << duration << "s" << std::endl;
    }

    double avg_duration = total_duration / bits;
    std::cout << "Average Duration over " << bits << " iterations, over " << particles.size() << " particles: " << avg_duration << "s" << std::endl;

    return 0;
}
