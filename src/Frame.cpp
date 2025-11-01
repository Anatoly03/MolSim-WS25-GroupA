#include "Frame.h"

#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>

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
            "Input File Format:\n"
            "  The input file contains the initial configuration of the particles.\n\n"
            "Options:\n"
            "  -o, --output <path>   output path, file name after last slash (default: ./MD_vtk, example: "
            "path/to/output/vtk)\n"
            "  -t, --time <int>      total simulation time (default: 1000)\n"
            "  -d, --delta <float>   time step delta (default: 0.014)\n"
            "  -L <level>            log level (hierarchy: trace, debug, info, warn, err, critical)\n"
            "  -h, --help            print this help message\n\n"
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
    fprintf(stderr,
            "Usage: %s [input] [options]\n"
            "Help:  %s -h\n",
            progname, progname);
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
    int log_level = 0;

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
            case 'o':
                args.output_path = const_cast<char *>(optarg);
                break;
            case 'L':
                log_level = atoi(optarg);

                // if log level is in valid range, set it directly in spdlog
                if (log_level >= 0 && log_level <= 5) {
                    spdlog::set_level(static_cast<spdlog::level::level_enum>(log_level));
                    break;
                }

                // TODO non-numeric log level name options like `-L warn` and `-L err` (see spdlog source code)
                // spdlog::level::from_str(&log_level_code);
                // spdlog::set_level(level);
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
        spdlog::error("missing positional argument: input file");
        printUsage(progname);
    }

    // preprocess output option if not provided
    if (args.output_path == nullptr) {
        args.output_path = const_cast<char *>("MD_vtk");
    } else {
        if (!createPath(args.output_path)) {
            spdlog::error("could not create path: {}", args.output_path);
            printUsage(progname);
        }
    }

    return args;
}
