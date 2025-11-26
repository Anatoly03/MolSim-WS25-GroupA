/**
 * The frame program of molsim.
 */

#pragma once

#include "../core/math/Vec3.h"
#include "../core/utils/Args.h"

#include <getopt.h>
#include <iostream>

const char *const OPTSTRING = "h:o:t:d:L:B:V:";

/**
 * @brief Definition of all long options.
 */
const option GETOPT_LONG[] = {{"output", required_argument, 0, 'o'},
                              {"time", required_argument, 0, 't'},
                              {"delta", required_argument, 0, 'd'},
                              {"help", optional_argument, 0, 'h'},
                              {0, 0, 0, 0}};

/**
 * @details The program entry point is the Rahmenprogramm which after getting
 * all variables calls the molecular simulation methods.
 */
Args ProcessArgs(int argc, char *argv[]);
