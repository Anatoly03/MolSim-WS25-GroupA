// Read more on Yaml:
// https://github.com/jbeder/yaml-cpp/wiki/Tutorial

#pragma once

#include "../ParticleContainer.h"
#include "../CuboidGenerator.h"
#include "../math/Vec3.h"
#include "../Particle.h"
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
    void readCuboidNode(ParticleContainer &particles, const YAML::Node &node) const {
        auto amount = node["n"].as<Vec3<int>>();
        Vec3D position = node["position"].as<Vec3<double>>();
        Vec3D velocity = node["velocity"].as<Vec3<double>>();
        double mass = node["mass"].as<double>();
        double brownian_sigma = node["brownian_sigma"].as<double>();
        
        double h;
        if (node["sigma"]) {
            double sigma = node["sigma"].as<double>();
            h = std::pow(2.0, 1.0 / 6.0) * sigma;
        } else {
            h = node["h"].as<double>();
        };

        Cuboid cuboid;

        cuboid.position = position;
        cuboid.n1 = amount.x;
        cuboid.n2 = amount.y;
        cuboid.n3 = amount.z;
        cuboid.h = h;
        cuboid.mass = mass;
        cuboid.initial_velocity = velocity;

        addCuboid2D(particles, cuboid, brownian_sigma);
    }

    /**
     * @brief Get particle from YAML::Node
     */
    void readNode(ParticleContainer &particles, const YAML::Node &node) const {
        std::string node_type = node["type"] ? (node["type"].as<std::string>()) : "particle";

        if (node_type == "cuboid") {
            return readCuboidNode(particles, node);
        }

        Particle particle;

        particle.position = node["position"].as<Vec3<double>>();
        particle.velocity = node["velocity"].as<Vec3<double>>();
        particle.mass = node["mass"].as<double>();

        particles.add(particle);
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

        // parse args/ simulation constants: cell size
        if (config["config"]["cell_size"]) {
            Vec3I value = config["config"]["cell_size"].as<Vec3I>();
            args.cell_size = value;
        }

        // parse args/ simulation constants: domain min
        if (config["config"]["domain_min"]) {
            Vec3I value = config["config"]["domain_min"].as<Vec3I>();
            args.domain_min = value;
        }

        // parse args/ simulation constants: domain max
        if (config["config"]["domain_max"]) {
            Vec3I value = config["config"]["domain_max"].as<Vec3I>();
            args.domain_max = value;
        }

        // parse args/ simulation constants: epsilon
        if (config["config"]["epsilon"]) {
            double value = config["config"]["epsilon"].as<double>();
            args.epsilon = value;
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
