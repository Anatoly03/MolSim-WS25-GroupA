#include "Frame.h"

/**
 * @brief Print help message and exit
 */
[[noreturn]]
void printHelp(const char * progname) {
    fprintf(stderr,
        "Usage:\n"
        "  %s [input] [options]\n\n"
        "Input File Format:\n"
        "  The input file contains the initial configuration of the particles.\n\n"
        "Options:\n"
        "  -t, --time <int>      total simulation time (default: 1000)\n"
        "  -d, --delta <float>   time step delta (default: 0.014)\n"
        "  -h, --help            print this help message\n",
        progname
    );
    exit(1);
}

/**
 * @brief Print usage message and exit. A short version of print help
 * used only when an error occurs in program frame.
 */
[[noreturn]]
void printUsage(const char * progname) {
    fprintf(stderr,
        "Usage: %s [file]\n",
        progname
    );
    exit(1);
}

/**
 * @brief Process command line arguments and return Args struct.
 * @param argc Argument count
 * @param argv CLI options
 * @return Parsed {@link Args} struct
 */
Args ProcessArgs(int argc, char *argv[]) {
    const char *progname = argv[0];
    Args args = Args();
    
    int opt;
    // parse options first
    while ((opt = getopt_long(argc, argv, OPTSTRING, GETOPT_LONG, nullptr)) != -1) {
        switch (opt)
        {
        case 't':
            args.end_time = atof(optarg);
            break;
        case 'd':
            args.delta_t = atof(optarg);
            break;

        case 'h': // -h or --help
        case '?': // unrecognized option
        default:
            printHelp(progname);
            break;
        }
    }

    // retrieve first positional argument: input file
    if (optind < argc) {
        args.input_file = argv[optind];
        optind++;
    } else {
        fprintf(stderr, "missing positional argument: input file");
        printUsage(progname);
    }

    return args;
}
