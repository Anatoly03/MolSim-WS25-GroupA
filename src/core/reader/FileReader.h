
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

//    protected:
//     /**
//      * @brief 
//      */
//     ParticleContainer particles;

   public:
    /**
     * @note Default constructor without providing particle container is private.
     */
    FileReader() = default;

    /**
     * @brief Destructor.
     */
    ~FileReader() = default;

    /**
     * @brief Get an input file stream for a specific file. Asserts
     * that the file could be opened and read out with `getline`.
     */
    virtual std::ifstream getFile(const char* filename) const {
        std::ifstream input_file(filename);

        // check if file could be opened
        if (!input_file.is_open()) {
            spdlog::error("could not open file {}", filename);
            exit(-1);
        }

        return input_file;
    }

    /**
     * @brief Read particle information from a file into particle container
     * attribute.
     */
    virtual void readFile(ParticleContainer& particles, const char *filename) {
        throw std::runtime_error("unimplemented: FileReader::readFile");
    }

    /**
     * @brief Get a reader-based instance for a specific file.
     */
    static std::unique_ptr<FileReader> getReaderForFile(const std::string &fileName);
};
