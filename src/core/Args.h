
#pragma once

/**
 * @brief Structure to hold all program arguments.
 */
struct Args {
    char *input_file;
    char *output_path;

    double start_time = 0;
    double end_time = 1000;
    double delta_t = 0.014;

    bool benchmark_enabled = false;
    int benchmark_iterations = 0;

    Args() = default;
    ~Args() = default;
};
