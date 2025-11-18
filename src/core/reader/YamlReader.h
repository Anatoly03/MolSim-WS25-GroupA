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

        // load yaml file
        try {
            config = YAML::LoadFile(filename);
        } catch (const YAML::BadFile &e) {
            spdlog::error("could not open yaml file {}: {}", filename, e.what());
            exit(-1);
        } catch (const YAML::ParserException &e) {
            spdlog::error("could not parse yaml file {}: {}", filename, e.what());
            exit(-1);
        }

        // parse args/ simulation constants: delta time
        if (config["config"]["delta_time"]) {
            double delta_time = config["config"]["delta_time"].as<double>();

            if (args.delta_t_cli) {
                spdlog::warn("delta_time in {} overridden by CLI argument: {} -> {}", filename, delta_time, args.delta_t);
            } else {
                args.delta_t = delta_time;
            }
        }

        // parse args/ simulation constants: duration
        if (config["config"]["delta_time"]) {
            double duration = config["config"]["total_time"].as<double>();

            if (args.delta_t_cli) {
                spdlog::warn("total_time in {} overridden by CLI argument: {} -> {}", filename, duration, args.end_time);
            } else {
                args.end_time = duration;
            }
        }

        // parse args/ simulation constants: output interval
        if (config["config"]["output_interval"]) {
            int interval = config["config"]["output_interval"].as<int>();
            args.output_interval = interval;
        }

        // parse particles
        if (!config["particles"]) {
            spdlog::error("yaml file {} has no 'particles' entry", filename);
            exit(-1);
        }

        YAML::Node particlesNode = config["particles"];

        // TODO extract YAML data
        (void) particles;
    }
};
