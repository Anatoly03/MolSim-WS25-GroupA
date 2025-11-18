// Read more on Yaml:
// https://github.com/jbeder/yaml-cpp/wiki/Tutorial

#pragma once

#include "../ParticleContainer.h"
#include "../math/Vec3.h"
#include "FileReader.h"

#include <fstream>
#include <iomanip>
#include <sstream>

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
     * @brief Get particle from YAML::Node
     */
    // TODO Specification of the cuboids.
    void readNode(ParticleContainer &particles, const YAML::Node &node) const {
        Vec3D position = Vec3D(
            node["position"][0].as<double>(),
            node["position"][1].as<double>(),
            node["position"][2].as<double>());

        Vec3D velocity = Vec3D(
            node["velocity"][0].as<double>(),
            node["velocity"][1].as<double>(),
            node["velocity"][2].as<double>());

        double mass = node["mass"].as<double>();

        particles.emplace_back(position, velocity, mass, 0);
    }

    /**
     * @brief Read custom 'text' file format into particle container.
     */
    virtual void readFile(ParticleContainer &particles, Args &args) override {
        YAML::Node config;

        // load yaml file
        try {
            config = YAML::LoadFile(args.input_file);
        } catch (const YAML::BadFile &e) {
            spdlog::error("could not open yaml file {}: {}", args.input_file, e.what());
            exit(-1);
        } catch (const YAML::ParserException &e) {
            spdlog::error("could not parse yaml file {}: {}", args.input_file, e.what());
            exit(-1);
        }

        // parse args/ simulation constants: delta time
        if (config["config"]["delta_time"]) {
            double delta_time = config["config"]["delta_time"].as<double>();

            if (args.delta_t_cli) {
                spdlog::warn("delta_time in {} overridden by CLI argument: {} -> {}", args.input_file, delta_time, args.delta_t);
            } else {
                args.delta_t = delta_time;
            }
        }

        // parse args/ simulation constants: duration
        if (config["config"]["delta_time"]) {
            double duration = config["config"]["total_time"].as<double>();

            if (args.delta_t_cli) {
                spdlog::warn("total_time in {} overridden by CLI argument: {} -> {}", args.input_file, duration, args.end_time);
            } else {
                args.end_time = duration;
            }
        }

        // parse args/ simulation constants: duration
        if (config["config"]["output"]) {
            std::string output_path = config["config"]["output"].as<std::string>();

            if (args.output_file_cli) {
                spdlog::warn("output path in {} overridden by CLI argument: `{}` -> `{}`", args.input_file, output_path, args.output_path);
            } else {
                args.output_path = output_path;
            }
        }

        // parse args/ simulation constants: output interval
        if (config["config"]["output_interval"]) {
            int interval = config["config"]["output_interval"].as<int>();
            args.output_interval = interval;
        }

        // parse particles
        if (!config["particles"]) {
            spdlog::error("yaml file {} has no 'particles' entry", args.input_file);
            exit(-1);
        }

        // read out particles
        YAML::Node node = config["particles"];

        if (node.IsSequence()) {
            for (YAML::const_iterator it=node.begin();it!=node.end();++it) {
                readNode(particles, *it);
            }
        } else if (node.IsMap()) {
            for(YAML::const_iterator it=node.begin();it!=node.end();++it) {
                readNode(particles, it->second);
            }
        }
    }
};
