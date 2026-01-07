/**
 * The frame program of molsim.
 */

#pragma once

#include <getopt.h>

#include <iostream>

const char *const OPTSTRING = "ho:t:d:c:k:D:g:N:T:X:Y:p:s:e:r:i";

/**
 * @brief Definition of all long options.
 */
const option GETOPT_LONG[] = {{"output", required_argument, 0, 'o'},
                              {"time", required_argument, 0, 't'},
                              {"delta", required_argument, 0, 'd'},
                              {"checkpoint", required_argument, 0, 'c'},
                              {"checkpoint-time", required_argument, 0, 'k'},
                              {"ggrav", required_argument, 0, 'g'},
                              {"add-disc", required_argument, 0, 'D'},
                              {"nthermostat", required_argument, 0, 'N'},
                              {"tinit", required_argument, 0, 'T'},
                              {"Lx", required_argument, 0, 'X'},
                              {"Ly", required_argument, 0, 'Y'},
                              {"periodic-lr", no_argument, 0, 'p'},
                              {"no-io", no_argument, 0, 'i'},
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
    // optional checkpoint file path to write a full phase-space snapshot
    char *checkpoint_path = nullptr;
    // time at which to write checkpoint (if checkpoint_path given). Use negative for disabled.
    double checkpoint_time = -1;
    // gravity acceleration (default: 0 -> no external gravity). Use negative value to point downwards in y.
    double ggrav = 0.0;
    // thermostat parameters
    int nthermostat = 1000;
    double Tinit = 0.5;
    // domain size (defaults from the exercise)
    double Lx = 303.0;
    double Ly = 180.0;
    // whether left/right boundaries are periodic
    bool periodic_lr = false;
    // optional disc arguments to add at restart: negative radius means disabled
    double disc_center_x = -1;
    double disc_center_y = -1;
    double disc_radius = -1;

    // Lennard-Jones parameters
    double sigma = 1.2;
    double epsilon = 1.0;
    double rcut = 2.5 * 1.2;  // default 2.5*sigma

    // disable all I/O (plots / checkpoint) for benchmarking
    bool no_io = false;

    Args() = default;
    ~Args() = default;
};

/**
 * @details The program entry point is the Rahmenprogramm which after getting
 * all variables calls the molecular simulation methods.
 */
Args ProcessArgs(int argc, char *argv[]);
