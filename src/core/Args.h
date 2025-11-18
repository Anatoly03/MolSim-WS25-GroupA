
#pragma once

/**
 * @brief Structure to hold all program arguments.
 */
struct Args {
    /**
     * @brief Input file name. Provided by CLI.
     */
    char *input_file;

    /**
     * @brief Output file path. Provided by CLI.
     */
    char *output_path;

    /**
     * @brief Simulation start time. Constant.
     */
    double start_time = 0;

    /**
     * @brief Simulation duration. Provided by CLI.
     */
    double end_time = 1000;

    /**
     * @brief Time step size. Provided by CLI.
     */
    double delta_t = 0.014;

    /**
     * @brief Benchmark flag.
     */
    bool benchmark_enabled = false;

    /**
     * @brief Number of iterations for benchmarking.
     */
    int benchmark_iterations = 0;

    /**
     * @brief Default constructor.
     */
    Args() = default;

    /**
     * @brief Default destructor.
     */
    ~Args() = default;
};
