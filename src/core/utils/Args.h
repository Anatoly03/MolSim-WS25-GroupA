
#pragma once

#include <string>

#include "../math/Vec3.h"

#include "spdlog/spdlog.h"

/**
 * @brief Structure to hold all program arguments.
 */
struct Args {
    /**
     * @brief Input file name. Provided by CLI.
     */
    char *input_file;

    /**
     * @brief Output file path. Provided by CLI or input files.
     */
    std::string output_path;

    /**
     * @brief True if the output file path was overriden by CLI argument.
     */
    bool output_file_cli = false;

    /**
     * @brief Log level for benchmarking output.
     */
    spdlog::level::level_enum log_level = spdlog::level::info;

    /**
     * @brief True if the log level was set by CLI argument.
     */
    bool log_level_cli = false;

    /**
     * @brief Simulation start time. Constant. Reserved for future use.
     */
    double start_time = 0;

    /**
     * @brief Simulation duration. Provided by CLI or input files.
     */
    double end_time = 1000;

    /**
     * @brief True if the end time was overriden by CLI argument.
     */
    bool end_time_cli = false;

    /**
     * @brief Time step size. Provided by CLI or input files.
     */
    double delta_t = 0.014;

    /**
     * @brief True if the delta time was overriden by CLI argument.
     */
    bool delta_t_cli = false;

    /**
     * @brief Time step size. Provided by input files.
     */
    int output_interval = 10;

    /**
     * @brief Thermostat initial temperature (Tinit).
     */
    double initial_temperature = 0.0;

    /**
     * @brief Thermostat target temperature (Ttarget).
     */
    double target_temperature = 0.0;

    /**
     * @brief Thermostat temperature step (ΔT).
     */
    double delta_temperature = 0.0;

    /**
     * @brief Thermostat application interval (ntherm). 0 disables.
     */
    int ntherm = 0;

    /**
     * @brief Thermodynamics stats interval (steps). 0 disables measurement.
     */
    int stats_every = 1000;

    /**
     * @brief RDF bin width.
     */
    double rdf_dr = 0.1;

    /**
     * @brief RDF number of bins.
     */
    int rdf_bins = 200;

    /**
     * @brief Cell size for linked cell implementation. Provided by input files.
     */
    Vec3I cell_size = Vec3I(5, 5, 5);

    /**
    * @brief epsilon for lennard-Jones Parameters.
    */
    double epsilon = 1.0;

    // TODO document
    double sigma = 1.0;

    // TODO document
    double cutoff_radius = 3.0;

    /**
     * @brief Lower smoothing radius `r_l` for smoothed Lennard-Jones potential.
     * Default chosen to typical LJ cutoff scale.
     */
    double smoothing_radius_lower = 2.5;

    /**
     * @brief Attraction method to use. Provided by input files.
     */
    std::string attraction_method = "lennard-jones";

    /**
     * @brief Cell size for linked cell implementation. Provided by input files.
     */
    Vec3I domain_min = Vec3I(0);

    /**
     * @brief Cell size for linked cell implementation. Provided by input files.
     */
    Vec3I domain_max = Vec3I(10);

    /**
     * @brief the boarder handling for each side.
     * Delete when out of bounds: 0
     * reflective: 1
     * periodic: 2
     * outflow: 3 <---need working
     */
    int boarderXmin = 1;

    int boarderXmax = 1;

    int boarderYmin = 1;

    int boarderYmax = 1;

    int boarderZmin = 1;

    int boarderZmax = 1;

    /**
     * @brief Default factor of the gravity to use. Overridden by CLI.
     */
    double gravityFactor = 0;

    /**
     * @brief Optional checkpoint input path.
     */
    std::string checkpoint_input;

    /**
     * @brief Optional checkpoint output path.
     */
    std::string checkpoint_output;

    /**
     * @brief Benchmark flag. Set by CLI.
     */
    bool benchmark_enabled = false;

    /**
     * @brief Number of iterations for benchmarking. Set by CLI.
     */
    int benchmark_iterations = 0;

    /**
     * @brief Default version of the algorithm to use. Overridden by CLI.
     */
    int version = 1;

    /**
     * @brief OpenMP parallelization strategy. 
     * 0 = Direct O(n^2) parallelization with thread-local accumulators
     * 1 = Cell-based O(n) parallelization with atomic updates (default)
     * Provided by input files.
     */
    int parallelization_strategy = 1;

    /**
     * @brief Default constructor.
     */
    Args() = default;

    /**
     * @brief Default destructor.
     */
    ~Args() = default;
};
