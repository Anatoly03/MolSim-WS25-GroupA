
#pragma once

#include <fstream>
#include <iomanip>
#include <sstream>

#include "../ParticleContainer.h"
#include "spdlog/spdlog.h"

/**
 * @brief Base class for all input readers. This is the entry point for a reader
 * that can read file type and yield a reader subtype, that can read particles.
 */
class FileReader {
   private:
    // TODO read state

   protected:
    std::unique_ptr<std::ifstream> input_file;
    //     /**
    //      * @brief
    //      */
    //     ParticleContainer particles;

   public:
    /**
     * @note Default constructor without providing particle container is private.
     */
    FileReader() : input_file(nullptr) {}

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
    virtual void readFile(ParticleContainer &particles, const char *filename) {
        claimFile(filename);

        // read magic header
        if (!readMagicHeader()) {
            spdlog::error("file {} has invalid magic header", filename);
            exit(-1);
        }

        // TODO abstraction
        (void) particles;
    };

    /**
     * @brief Get a reader-based instance for a specific file.
     */
    static std::unique_ptr<FileReader> getReaderForFile(const std::string &fileName);
};
