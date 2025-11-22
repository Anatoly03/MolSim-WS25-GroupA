
#pragma once

#include <fstream>
#include <iomanip>
#include <sstream>

#include "../ParticleContainer.h"
#include "../Args.h"
#include "spdlog/spdlog.h"

/**
 * @brief Base class for all input readers. This is the entry point for a reader
 * that can read file type and yield a reader subtype, that can read particles.
 */
class FileReader {
   protected:
    /**
     * CLI input arguments, can be overriden by various file formats.
     */
    Args *args;

    /**
     * @brief Input file stream.
     */
    std::unique_ptr<std::ifstream> input_file;
    //     /**
    //      * @brief
    //      */
    //     ParticleContainer particles;

   public:
    /**
     * @note Default constructor without providing particle container is private.
     */
    FileReader() : args(nullptr), input_file(nullptr) {}

    /**
     * @brief Virtual destructor.
     */
    virtual ~FileReader() = default;

    /**
     * @brief Get an input file stream for a specific file. Asserts
     * that the file could be opened and read out with `getline`.
     */
    virtual void claimFile(const char *filename) {
        input_file = std::make_unique<std::ifstream>(filename);

        // check if file could be opened
        if (!input_file->is_open()) {
            spdlog::error("could not open file {}", filename);
            exit(-1);
        }

        // success!
    }

    /**
     * @brief Read particle information from a file into particle container
     * attribute.
     */
    virtual void readLine(std::string &line) const { getline(*input_file, line); }

    /**
     * @brief Read magic header. Per default file formats have no magic
     * header, so this always returns true.
     */
    virtual bool readMagicHeader() const { return true; }

    /**
     * @brief Read particle information from a file into particle container
     * attribute.
     */
    // NOLINTNEXTLINE(unused-parameter)
    virtual void readFile(ParticleContainer &particles, Args &args) {
        claimFile(args.input_file);

        // read magic header
        if (!readMagicHeader()) {
            spdlog::error("file {} has invalid magic header", args.input_file);
            exit(-1);
        }

        // TODO abstraction
        (void) particles;
    };

    //
    // STATIC
    //

    /**
     * @brief Get a reader-based instance for a specific file.
     */
    static std::unique_ptr<FileReader> getReaderForFile(const std::string &fileName);

    /**
     * @brief Read file and 
     */
    static std::unique_ptr<FileReader> writeParticles(ParticleContainer &particles, Args &args) {
        std::unique_ptr<FileReader> fileReader = FileReader::getReaderForFile(args.input_file);
        fileReader->readFile(particles, args);
        return fileReader;
    }
};
