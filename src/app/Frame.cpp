#include "Frame.h"

#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "../core/utils/Args.h"
#include "spdlog/spdlog.h"

// https://stackoverflow.com/a/42906151 mkdir needs wrapper on windows subsystems
#ifdef _WIN32
#include <direct.h>
#define mkdir(path, mode) _mkdir(path)
#endif

/**
 * @brief Print help message and exit.
 * @note DO NOT USE SPDLOG HERE. This is part of the program frame and
 * should be printed conventionally using fprintf.
 */
[[noreturn]]
void printHelp(const char *progname) {
    fprintf(stdout,
            "Usage:\n"
            "  %s [input] [options]\n\n"

            "Allowed Input File Formats:\n\n"

            "  === Text File: .txt, .text ===\n"
            "    Deprecated. Use YAML format instead.\n"
            "  === YAML File: .yaml, .yml ===\n"
            "    Recommended file format for input configuration.\n\n"
            "    name: string (simulation name)\n"
            "    config:\n"
            "      delta_time: float (time step delta)\n"
            "      total_time: float (total simulation time)\n"
            "      output: string (output file path, default: ./MD_vtk)\n"
            "      output_interval: int (output interval in number of steps)\n"
            "    particles: (map of named particles or list of particles)\n"
            "      position: vec3 (particle initial position)\n"
            "      velocity: vec3 (particle initial velocity)\n"
            "      mass: float (particle mass)\n\n"

            "Options:\n"
            "  -o, --output <path>   output path, file name after last slash (default: ./MD_vtk, example: "
            "path/to/output/vtk)\n"
            "  -t, --time <int>      total simulation time (default: 1000)\n"
            "  -d, --delta <float>   time step delta (default: 0.014)\n"
            "  -L <level>            spdlog level\n"
            "                        hierarchy: trace - 0, debug - 1, info - 2, warn, err, critical, off\n"
            "                        note: in benchmark mode this applies to benchmark logs only\n"
            "  -B <amount>           benchmark parameter, if specified will re-run simulation and output benchmark "
            "results\n"
            "  -V <amount>           algorithm version parameter\n"
            "         V0             Direct Sum Algorithm\n"
            "         V1 (default)   Linked Cell Algorithm\n"
            "  --help                print this help message\n"
            "  --help short          print compact help message\n\n"

            "Example:\n"
            "  %s input.txt -o output/simulation -t 500 -d 0.01\n",
            progname, progname);
    exit(1);
}

/**
 * @brief Print usage message and exit. A short version of print help
 * used only when an error occurs in program frame.
 * @note DO NOT USE SPDLOG HERE. This is part of the program frame and
 * should be printed conventionally using fprintf.
 */
[[noreturn]]
void printUsage(const char *progname) {
    fprintf(stderr, "Usage: %s --help\n", progname);
    exit(1);
}

/**
 * @brief Preprocess output option to create the folders if not
 * existing and retrieve pattern.
 * @return True if successful, false in case of an error.
 * @details Recursive function creating all necessary folders. This
 * should be run directly after the project arguments have been processed.
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
            case 't':  // -t or --time
                args.end_time = atof(optarg);
                args.end_time_cli = true;
                break;
            case 'd':  // -d or --delta
                args.delta_t = atof(optarg);
                args.delta_t_cli = true;
                break;
            case 'o':  // -o or --output
                args.output_path = std::string(optarg);
                args.output_file_cli = true;
                break;
            case 'L':
                args.log_level_cli = true;
                args.log_level = static_cast<spdlog::level::level_enum>(atoi(optarg));

                // TODO non-numeric log level name options like `-L warn` and `-L err` (see spdlog source code)
                // args.log_level = spdlog::level::from_str(optarg);

                break;
            case 'B':
                args.benchmark_enabled = true;
                args.benchmark_iterations = atoi(optarg);
                break;
            case 'V':
                args.version = atoi(optarg);

                switch (args.version) {
                    case 0:
                    case 1:
                        break;

                    default:
                        spdlog::error("invalid algorithm version specified: {}", args.version);
                        printUsage(progname);
                }

                break;

            case 'h':  // -h or --help
                if (optarg != nullptr && strcmp(optarg, "short") == 0) {
                    printUsage(progname);
                }

                printHelp(progname);
                break;

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
        spdlog::error("missing positional argument: input file");
        printUsage(progname);
    }

    // preprocess output option if not provided
    if (args.output_path.empty()) {
        args.output_path = "MD_vtk";
    } else {
        if (!createPath(args.output_path.c_str())) {
            spdlog::error("could not create path: {}", args.output_path);
            printUsage(progname);
        }
    }

    // SUCCESS !
    // NO MORE FURTHER ERRORS

    spdlog::set_level(args.log_level);

    // print version of implementation
    switch (args.version) {
    case 0:
        spdlog::debug("running `direct sum` implementation");
        break;
    case 1:
        spdlog::debug("running `linked cell` implementation");
        break;
    }

    // disable all further logging in benchmark mode
    if (args.benchmark_enabled) {
        if (args.benchmark_iterations == 0) {
            spdlog::error("benchmark iterations (-B {}) must be greater than zero", args.benchmark_iterations);
            printUsage(progname);
        }

        // default logging is trace of benchmark iterations
        if (!args.log_level_cli) {
            args.log_level = spdlog::level::trace;
        }

        spdlog::debug("benchmark: running {} iterations...", args.benchmark_iterations);
        spdlog::set_level(spdlog::level::err); // this will be overriden by custom benchmark implementation
    }

    return args;
}
