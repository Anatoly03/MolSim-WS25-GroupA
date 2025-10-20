/**
 * The frame program of molsim.
 */

#pragma once

#include <getopt.h>
#include <iostream>

const char* const OPTSTRING = "ho:";

/**
 * Definition of all long options.
 */
const option GETOPT_LONG[] = {
    {"output", required_argument, 0, 'o'},
    {"help", no_argument, 0, 'h'},
    {0, 0, 0, 0}
};

/**
 * Structure to hold all program arguments.
 */
struct Args {
    char* input_file;
    const char* output_dir;

    double start_time = 0;
    double end_time = 100; // 1000
    double delta_t = 0.014;

    // constructor with default values
    Args() : input_file(NULL), output_dir(NULL), start_time(0), end_time(100), delta_t(0.014) {}
};

const char* const DEFAULT_OUTPUT_DIR = "gen";

/**
 * The program entry point is the Rahmenprogramm which after getting all variables
 * calls the molecular simulation methods.
 */
Args ProcessArgs(int argc, char *argv[]);
