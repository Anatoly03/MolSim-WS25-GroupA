#include "Frame.h"

#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>

// https://stackoverflow.com/a/42906151 mkdir needs wrapper on windows subsystems
#ifdef _WIN32
#include <direct.h>
#define mkdir(path, mode) _mkdir(path)
#endif

/**
 * @brief Print help message and exit
 */
[[noreturn]]
void printHelp(const char *progname) {
    fprintf(stderr,
            "Usage:\n"
            "  %s [input] [options]\n\n"
            "Input File Format:\n"
            "  The input file contains the initial configuration of the particles.\n\n"
            "Options:\n"
            "  -o, --output <path>   output path, file name after last slash (default: ./MD_vtk)\n"
            "  -t, --time <int>      total simulation time (default: 1000)\n"
            "  -d, --delta <float>   time step delta (default: 0.014)\n"
            "  -c, --checkpoint <path>   write checkpoint file to <path> when --checkpoint-time is reached\n"
            "  -k, --checkpoint-time <float>   time at which checkpoint is written (default: disabled)\n"
            "  -g, --ggrav <float>   gravity acceleration (default: 0.0, negative points downwards)\n"
            "  -D, --add-disc <x,y,r>   add a disc of particles centered at x,y with radius r\n"
            "  -N, --nthermostat <int>   apply thermostat every N iterations (0 to disable, default: 1000)\n"
            "  -T, --tinit <float>   target temperature for thermostat (default: 0.5)\n"
            "  --Lx <float>   domain size in x (default: 303)\n"
            "  --Ly <float>   domain size in y (default: 180)\n"
            "  -p, --periodic-lr       set left/right boundaries to periodic (default: reflective)\n"
            "  -s, --sigma <float>    Lennard-Jones sigma (default: 1.2)\n"
            "  -e, --epsilon <float>  Lennard-Jones epsilon (default: 1.0)\n"
            "  -r, --rcut <float>     cutoff radius for LJ (default: 2.5*sigma)\n"
            "  -i, --no-io            disable all I/O (plots/checkpoint) for benchmarking\n"
            "  -h, --help            print this help message\n\n"
            "Example:\n"
            "  %s input.txt -o output/simulation -t 500 -d 0.01\n",
            progname, progname);
    exit(1);
}

/**
 * @brief Print usage message and exit. A short version of print help
 * used only when an error occurs in program frame.
 */
[[noreturn]]
void printUsage(const char *progname) {
    fprintf(stderr, "Usage: %s [input] [options]\nTry '%s --help' for more information.\n", progname, progname);
    exit(1);
}

/**
 * @brief Preprocess output option to create the folders if not
 * existing and retrieve pattern.
 * @return True if successful, false in case of an error.
 * @details Recursive function creating all necessary folders.
 */
bool createPath(const char *output_pattern, const char *directory_offset = "") {
    std::string out_str(output_pattern);
    size_t first_slash = out_str.find_first_of("/\\");

    // the path has no folder anymore, it's the filename then
    if (first_slash == std::string::npos) {
        return true;
    }

    std::string folder(directory_offset + out_str.substr(0, first_slash + 1));

    // create folder if not existing
    if (mkdir(folder.c_str(), 0777) && errno != EEXIST) {
        return false;
    }

    // recursive call for next folder
    createPath(out_str.substr(first_slash + 1).c_str(), folder.c_str());

    return true;
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
        switch (opt) {
            case 't':
                args.end_time = atof(optarg);
                break;
            case 'd':
                args.delta_t = atof(optarg);
                break;
            case 'c':
                args.checkpoint_path = const_cast<char *>(optarg);
                break;
            case 'k':
                args.checkpoint_time = atof(optarg);
                break;
            case 'g':
                args.ggrav = atof(optarg);
                break;
            case 'D': {
                // parse three values: cx, cy, R. Accept comma or space separated values
                double cx = 0, cy = 0, r = -1;
                // try comma-separated first
                if (sscanf(optarg, "%lf,%lf,%lf", &cx, &cy, &r) < 3) {
                    // fallback to space-separated
                    if (sscanf(optarg, "%lf %lf %lf", &cx, &cy, &r) < 3) {
                        fprintf(stderr, "error: invalid disc format '%s'. Use: x,y,r or 'x y r'\n", optarg);
                        printUsage(progname);
                    }
                }
                args.disc_center_x = cx;
                args.disc_center_y = cy;
                args.disc_radius = r;
                break;
            }
            case 'N':
                args.nthermostat = atoi(optarg);
                break;
            case 'T':
                args.Tinit = atof(optarg);
                break;
            case 'X':
                args.Lx = atof(optarg);
                break;
            case 'Y':
                args.Ly = atof(optarg);
                break;
            case 'p':
                args.periodic_lr = true;
                break;
            case 'o':
                args.output_path = const_cast<char *>(optarg);
                break;
            case 's':
                args.sigma = atof(optarg);
                break;
            case 'e':
                args.epsilon = atof(optarg);
                break;
            case 'r':
                args.rcut = atof(optarg);
                break;
            case 'i':
                args.no_io = true;
                break;

            case 'h':  // -h or --help
            case '?':  // unrecognized option
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
        fprintf(stderr, "error: missing positional argument: input file\n");
        printUsage(progname);
    }

    // preprocess output option if not provided
    if (args.output_path == nullptr) {
        args.output_path = const_cast<char *>("MD_vtk");
    }

    // create output path if necessary
    if (!createPath(args.output_path)) {
        fprintf(stderr, "error: could not create output path '%s'\n", args.output_path);
        exit(1);
    }

    return args;
}
