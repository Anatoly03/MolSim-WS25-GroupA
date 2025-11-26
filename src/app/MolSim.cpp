
#include <string.h>

#include "../core/ParticleContainer.h"
#include "../core/reader/FileReader.h"
#include "../core/simulation/Simulation.h"
#include "../core/simulation/DirectSumAlgorithm.h"
#include "Frame.h"

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
    auto args = ProcessArgs(argc, argsv);

    ParticleContainer particles;

    // If the input file name is "task4", run the simulation of 2D collision of two cuboids
    // Otherwise, use the regular file input.
    std::string input_name = args.input_file ? std::string(args.input_file) : "";

    // load particles from file
    auto reader = FileReader::writeParticles(particles, args);

    // set up simulation variable
    std::shared_ptr<Simulation> simulation = nullptr;

    // set up simulation
    if (!args.benchmark_enabled) {
        spdlog::set_level(args.log_level);
        simulation = Simulation::createSimulation(particles, args);

#ifdef ENABLE_VTK_OUTPUT
        auto writer = std::make_shared<outputWriter::VTKWriter>();
#else
        auto writer = std::make_shared<outputWriter::XYZWriter>();
#endif

        // everything ready - run the simulation
        simulation->run([&simulation, &writer, &args](int iteration, Simulation& sim){
            writer->plot(args.output_path, iteration, *simulation);
        });

        return 0;
    }

    // set up benchmarking
    const int bits = args.benchmark_iterations;

    timespec starttime;
    timespec end;
    double total_duration = 0.0;

    for (int i = 0; i < bits; i++) {
        spdlog::set_level(spdlog::level::off); // disable logging for benchmarking

        ParticleContainer copy(particles);
        simulation = Simulation::createSimulation(copy, args);
        
        clock_gettime(CLOCK_MONOTONIC, &starttime);
        simulation->run(nullptr);
        clock_gettime(CLOCK_MONOTONIC, &end);

        double duration = (double)(end.tv_sec - starttime.tv_sec) + ((double)(end.tv_nsec - starttime.tv_nsec) * 1e-9);
        total_duration += duration;

        spdlog::set_level(args.log_level); // end benchmark iteration: report
        spdlog::trace("benchmark: iteration {}: in {:.4f}s", i, duration);
        if (i == bits - 1) { // verify benchmark result is relevant during last measurement
            if (simulation->particleCount() != copy.size()) // how many particles are lost? this is important for measurement
                spdlog::warn("particle count changed during simulation: was {}, at end is {}", copy.size(), simulation->particleCount());
        }
        spdlog::set_level(spdlog::level::off); // disable logging for benchmarking (destructor invocation)
    }

    double avg_duration = total_duration / bits;
    spdlog::set_level(args.log_level); // end all benchmarking: report
    spdlog::debug("benchmark: finish {} iterations, over {} particles", bits, particles.size());
    spdlog::info("average: {:.4f}s", avg_duration);
    spdlog::info("total:   {:.4f}s", total_duration);
    spdlog::set_level(spdlog::level::off); // disable further logging (destructor invocation)
    return 0;
}
