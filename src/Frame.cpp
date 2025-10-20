#include "Frame.h"

[[noreturn]] void printHelp(const char * progname) {
    // TODO add meaningful help message
    printf(
        "Usage:\n"
        "  %s [input] -o [output]\n\n"
        "Options:\n"
        "  -o, --output <dir>    output directory\n"
        "  -h, --help            print this help message\n",
        progname
    );
    exit(1);
}

[[noreturn]] void printUsage(const char * progname) {
    // TODO add meaningful help message
    printf(
        "Usage: %s [file] -o [dir]\tapply molecular simulation to input <file> and produce output in <dir>\n",
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
            args.output_dir = optarg;
            printf("Output directory set to: %s\n", args.output_dir);
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
    }

    // if output directory is not set, use default
    if (args.output_dir == NULL) {
        args.output_dir = DEFAULT_OUTPUT_DIR;
    }

    return args;
}
