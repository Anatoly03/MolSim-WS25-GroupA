#include "Frame.h"

/**
 * @brief Print help message and exit
 */
[[noreturn]]
void printHelp(const char * progname) {
    fprintf(stderr, 
        "Usage:\n"
        "  %s [input] -o [output]\n\n"
        "Options:\n"
        "  -o, --output <dir>    output directory\n"
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
        "Usage: %s [file] \t apply molecular simulation to input <file> and produce output in <dir>\n",
        progname
    );
    exit(1);
}

// process command line arguments and return Args struct
Args ProcessArgs(int argc, char *argv[]) {
    const char *progname = argv[0];
    Args args = Args();
    
    int opt;
    // parse options first
    while ((opt = getopt_long(argc, argv, OPTSTRING, GETOPT_LONG, nullptr)) != -1) {
        switch (opt)
        {
        case 'o':
            // TODO input validation
            args.output_dir = optarg;
            break;
        case 't':
            // TODO input validation
            args.end_time = atof(optarg);
            break;
        case 'd':
            // TODO input validation
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

    // if output directory is not set, use default
    if (args.output_dir == NULL) {
        args.output_dir = DEFAULT_OUTPUT_DIR;
    }

    return args;
}
