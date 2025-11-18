// Read more on Yaml:
// https://github.com/jbeder/yaml-cpp/wiki/Tutorial

#pragma once

#include <fstream>
#include <iomanip>
#include <sstream>

#include "../ParticleContainer.h"
#include "../math/Vec3.h"
#include "FileReader.h"
#include "spdlog/spdlog.h"
#include "yaml-cpp/yaml.h"

/**
 * @brief Base class for all input readers. This is the entry point for a reader
 * that can read file type and yield a reader subtype, that can read particles.
 */
class YamlReader : public FileReader {
   public:
    /**
     * @note Default constructor without providing particle container is private.
     */
    YamlReader() = default;

    /**
     * @brief Destructor.
     */
    ~YamlReader() = default;

    /**
     * @brief Read custom 'text' file format into particle container.
     */
    virtual void readFile(ParticleContainer &particles, const char *filename) override {
        YAML::Node config;

        try {
            config = YAML::LoadFile(filename);
        } catch (const YAML::BadFile &e) {
            spdlog::error("could not open yaml file {}: {}", filename, e.what());
            exit(-1);
        } catch (const YAML::ParserException &e) {
            spdlog::error("could not parse yaml file {}: {}", filename, e.what());
            exit(-1);
        }

        YAML::Emitter out;
        out << config;
        spdlog::debug("yaml file decoded: {}", out.c_str());

        // TODO extract YAML data
        (void) particles;
    }
};
