
#include "Frame.h"
#include "../core/ParticleContainer.h"
#include "../core/Simulation.h"
#include "../core/reader/FileReader.h"
#include "../core/CuboidGenerator.h"
#include "spdlog/spdlog.h"

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

    // If the input file name is "task4", run the simulation of 2D collision of two cuboids
    // Otherwise, use the regular file input.
    std::string input_name = args.input_file ? std::string(args.input_file) : "";

    if (input_name == "task4") {
        const double sigma = 1.0;
        const double h     = std::pow(2.0, 1.0 / 6.0) * sigma;
        const double mass  = 1.0;
        const double brownian_sigma = 0.1;

        Cuboid c1;
        c1.position         = Vec3D{0.0, 0.0, 0.0};
        c1.n1 = 40; c1.n2 = 8; c1.n3 = 1;
        c1.h                = h;
        c1.mass             = mass;
        c1.initial_velocity = Vec3D{0.0, 0.0, 0.0};

        Cuboid c2;
        c2.position         = Vec3D{15.0, 15.0, 0.0};
        c2.n1 = 8; c2.n2 = 8; c2.n3 = 1;
        c2.h                = h;
        c2.mass             = mass;
        c2.initial_velocity = Vec3D{0.0, -10.0, 0.0};

        addCuboid2D(particles, c1, brownian_sigma);
        addCuboid2D(particles, c2, brownian_sigma);

    } else {
        (void) FileReader::writeParticles(particles, args);
    }


    // set up simulation
    if (!args.benchmark_enabled) {
        Simulation simulation(particles, args);

#ifdef ENABLE_VTK_OUTPUT
        outputWriter::VTKWriter writer(particles);
        simulation.setWriter(std::make_unique<outputWriter::VTKWriter>(particles));
#else
        outputWriter::XYZWriter writer(particles);
        simulation.setWriter(std::make_unique<outputWriter::XYZWriter>(particles));
#endif

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
        spdlog::info("Benchmark iteration {} finished in {:.4f}s", i, duration);
    }

    double avg_duration = total_duration / bits;
    spdlog::info("Average duration over {} iterations and {} particles: {:.4f}s",
                 bits, particles.size(), avg_duration);
    return 0;
}
