/**
 * The frame program of molsim.
 */

#pragma once

#include <getopt.h>

#include <iostream>

const char *const OPTSTRING = "ho:t:d:";

/**
 * @brief Definition of all long options.
 */
const option GETOPT_LONG[] = {{"output", required_argument, 0, 'o'},
                              {"time", required_argument, 0, 't'},
                              {"delta", required_argument, 0, 'd'},
                              {"help", no_argument, 0, 'h'},
                              {0, 0, 0, 0}};

/**
 * @brief Structure to hold all program arguments.
 */
struct Args {
    char *input_file;
    char *output_path;

    double start_time = 0;
    double end_time = 1000;
    double delta_t = 0.014;

    Args() = default;
    ~Args() = default;
};

/**
 * @details The program entry point is the Rahmenprogramm which after getting
 * all variables calls the molecular simulation methods.
 */
Args ProcessArgs(int argc, char *argv[]);
