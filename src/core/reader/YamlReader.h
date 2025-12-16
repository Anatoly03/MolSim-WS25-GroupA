// Read more on Yaml:
// https://github.com/jbeder/yaml-cpp/wiki/Tutorial

#pragma once

#include "../ParticleContainer.h"
#include "../generator/BallGenerator.h"
#include "../generator/CuboidGenerator.h"
#include "../generator/DiscGenerator.h"
#include "../physics/Force.h"
#include "../utils/Args.h"
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
   private:
    YAML::Node head = YAML::Node();

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
     * @brief Safe read: Read node oout of YAML file, with unwrap value.
     * Adjusted Rust.
     * @example
     * ```c++
     * unwrap_node()
     * ```
     */
    template<typename T, typename... K>
    T unwrap_node(T _default, K... keys) const {

        // https://github.com/jbeder/yaml-cpp/issues/348
        // https://stackoverflow.com/questions/11129234/copy-semantics-and-comparison
        // we don't care about performance in the method below

        // also f!ck c++, this method was marked as const yet it was modifying the
        // attribute `head` at runtime, the clang compiler failed us and this is annoying
        // and frustrating
        YAML::Node current = Clone(head);

        if constexpr (sizeof...(K) > 0) {
            // iterate over keys using crazy wizardry c++ macro syntax
            for (const auto &k : {std::string(keys)...}) {
                if (!current) break;
                current = current[k];
            }
        } else {
            // no keys provided, return default
            return _default;
        }

        // final unwrap_or
        if (current) {
            return current.as<T>();
        } else {
            return _default;
        }
    }

    /**
     * @brief Get particle from YAML::Node
     */
    void readCuboidNode(ParticleContainer &particles, const YAML::Node &node) const {
        auto amount = node["n"].as<Vec3<int>>();
        Vec3D position = node["position"].as<Vec3<double>>();
        Vec3D velocity = node["velocity"].as<Vec3<double>>();
        double mass = node["mass"].as<double>();
        double brownian_sigma = node["brownian_sigma"].as<double>();
        double epsilon = node["epsilon"].as<double>();

        double h;
        if (node["sigma"]) {
            double sigma = node["sigma"].as<double>();
            h = std::pow(2.0, 1.0 / 6.0) * sigma;

        } else {
            h = node["dist"].as<double>();
        };

        CuboidGenerator cuboid;

        cuboid.position = position;
        cuboid.size = amount;
        cuboid.spacing = Vec3D(h); // for later: allow rescale distance per axis
        cuboid.mass = mass;
        cuboid.initial_velocity = velocity;
        cuboid.brownian_sigma = brownian_sigma;
        cuboid.epsilon = epsilon;

        cuboid.generate(particles);
    }

    /**
     * @brief Get particle from YAML::Node
     */
    void readDiscNode(ParticleContainer &particles, const YAML::Node &node) const {
        int radius = node["radius"].as<int>();
        Vec3D center = node["center"].as<Vec3<double>>();
        Vec3D velocity = node["velocity"].as<Vec3<double>>();
        double mass = node["mass"].as<double>();
        double brownian_sigma = node["brownian_sigma"].as<double>();
        double spacing = node["dist"].as<double>();
        double epsilon = node["epsilon"].as<double>();


        DiscGenerator disc;

        disc.center = center;
        disc.radius = radius;
        disc.spacing = spacing;
        disc.mass = mass;
        disc.initial_velocity = velocity;
        disc.brownian_sigma = brownian_sigma;
        disc.epsilon = epsilon;


        disc.generate(particles);
    }

    /**
     * @brief Get particle from YAML::Node
     */
    void readBallNode(ParticleContainer &particles, const YAML::Node &node) const {
        int radius = node["radius"].as<int>();
        Vec3D center = node["center"].as<Vec3<double>>();
        Vec3D velocity = node["velocity"].as<Vec3<double>>();
        double mass = node["mass"].as<double>();
        double brownian_sigma = node["brownian_sigma"].as<double>();
        double spacing = node["dist"].as<double>();
        double epsilon = node["epsilon"].as<double>();


        BallGenerator ball;

        ball.center = center;
        ball.radius = radius;
        ball.spacing = spacing;
        ball.mass = mass;
        ball.initial_velocity = velocity;
        ball.brownian_sigma = brownian_sigma;
        ball.epsilon = epsilon;


        ball.generate(particles);
    }

    /**
     * @brief Get particle from YAML::Node
     */
    void readConfig(Args &args) const {
        const double delta_time = unwrap_node<double>(args.delta_t, "config", "delta_time");
        const double total_time = unwrap_node<double>(args.end_time, "config", "total_time");
        const std::string output_path = unwrap_node<std::string>(args.output_path, "config", "output");
        const int output_interval = unwrap_node<int>(args.output_interval, "config", "output_interval");
        const Vec3I cell_size = unwrap_node<Vec3I>(args.cell_size, "config", "cell_size");
        const Vec3I domain_min = unwrap_node<Vec3I>(args.domain_min, "config", "domain_min");
        const Vec3I domain_max = unwrap_node<Vec3I>(args.domain_max, "config", "domain_max");
        const double epsilon = unwrap_node<double>(args.epsilon, "config", "epsilon");
        const double sigma = unwrap_node<double>(args.sigma, "config", "sigma");
        const double cut_off = unwrap_node<double>(args.cutoff_radius, "config", "cut_off");
        const std::string attraction_method = unwrap_node<std::string>("lennard-jones", "config", "attraction");

        const std::string boarderXmin = unwrap_node<std::string>("reflect", "config", "boarderXmin");
        const std::string boarderXmax = unwrap_node<std::string>("reflect", "config", "boarderXmax");
        const std::string boarderYmin = unwrap_node<std::string>("reflect", "config", "boarderYmin");
        const std::string boarderYmax = unwrap_node<std::string>("reflect", "config", "boarderYmax");
        const std::string boarderZmin = unwrap_node<std::string>("reflect", "config", "boarderZmin");
        const std::string boarderZmax = unwrap_node<std::string>("reflect", "config", "boarderZmax");

        const double gravityFactor = unwrap_node<double>(args.gravityFactor, "config", "gravityFactor");



        if (args.delta_t_cli) {
            spdlog::warn("delta_time in {} overridden by CLI argument: {} -> {}", args.input_file, delta_time, args.delta_t);
        } else {
            args.delta_t = delta_time;
        }

        if (args.end_time_cli) {
            spdlog::warn("total_time in {} overridden by CLI argument: {} -> {}", args.input_file, total_time, args.end_time);
        } else {
            args.end_time = total_time;
        }

        if (args.output_file_cli) {
            spdlog::warn("output path in {} overridden by CLI argument: `{}` -> `{}`", args.input_file, output_path, args.output_path);
        } else {
            args.output_path = output_path;
        }

        args.output_interval = output_interval;
        args.cell_size = cell_size;
        args.domain_min = domain_min;
        args.domain_max = domain_max;
        args.epsilon = epsilon;
        args.sigma = sigma;
        args.cutoff_radius = cut_off;
        args.attraction_method = attraction_method;
        //std::cout<<boarderXmin<<std::endl;

        args.boarderXmin = boarderXmin;
        args.boarderXmax = boarderXmax;
        args.boarderYmin = boarderYmin;
        args.boarderYmax = boarderYmax;
        args.boarderZmin = boarderZmin;
        args.boarderZmax = boarderZmax;
        args.gravityFactor = gravityFactor;
    }

    /**
     * @brief Get particle from YAML::Node
     */
    void readNode(ParticleContainer &particles, const YAML::Node &node) const {
        std::string node_type = node["type"] ? (node["type"].as<std::string>()) : "particle";

        if (node_type == "cuboid") {
            return readCuboidNode(particles, node);
        }
        
        if (node_type == "ball") {
            return readBallNode(particles, node);
        }

        if (node_type == "disc") {
            return readDiscNode(particles, node);
        }

        Particle particle;

        particle.position = node["position"].as<Vec3<double>>();
        particle.velocity = node["velocity"].as<Vec3<double>>();
        particle.mass = node["mass"].as<double>();
        particle.sigma = node["sigma"].as<double>();
        particle.epsilon = node["epsilon"].as<double>();

        particles.add(particle);
    }

    /**
     * @brief Read custom 'text' file format into particle container.
     * @returns True on success, false on failure.
     */
    virtual bool readFile(ParticleContainer &particles, Args &args) override {
        try { // load yaml file
            head = YAML::LoadFile(args.input_file);
        } catch (const YAML::BadFile &e) { // IO error
            spdlog::error("could not open yaml file {}: {}", args.input_file, e.what());
            return false;
        } catch (const YAML::ParserException &e) {
            spdlog::error("could not parse yaml file {}: {}", args.input_file, e.what());
            return false;
        }

        // read global physics config
        readConfig(args);

        // parse particles
        if (!head["particles"]) {
            spdlog::error("yaml file {} has no 'particles' entry", args.input_file);
            return false;
        }

        // parse particles
        YAML::Node node = head["particles"];

        if (node.IsSequence()) {
            for (YAML::const_iterator it=node.begin();it!=node.end();++it) {
                readNode(particles, *it);
            }
        } else if (node.IsMap()) {
            for(YAML::const_iterator it=node.begin();it!=node.end();++it) {
                readNode(particles, it->second);
            }
        }

        return true;
    }
};
