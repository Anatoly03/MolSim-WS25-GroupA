// Read more on Yaml:
// https://github.com/jbeder/yaml-cpp/wiki/Tutorial

#pragma once

#include "../ParticleContainer.h"
#include "../Membrane.h"
#include "../generator/BallGenerator.h"
#include "../generator/CuboidGenerator.h"
#include "../generator/DiscGenerator.h"
#include "../generator/MembraneGenerator.h"
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
    void readCuboidNode(ParticleContainer &particles, const YAML::Node &node, Args &args) const {
        auto amount = node["n"].as<Vec3<int>>();
        Vec3D position = node["position"].as<Vec3<double>>();
        Vec3D velocity = node["velocity"].as<Vec3<double>>();
        double mass = node["mass"].as<double>();
        double brownian_sigma = node["brownian_sigma"].as<double>();


        double epsilon;


        if(node["epsilon"]) {
            epsilon = node["epsilon"].as<double>();
        }else{
            //std::cout<<"args eps "<<args.epsilon<<std::endl;
            epsilon = args.epsilon;
        }


        double h;
        CuboidGenerator cuboid;


        if (node["sigma"]) {
            double sigma = node["sigma"].as<double>();
            h = std::pow(2.0, 1.0 / 6.0) * sigma;
            cuboid.sigma = sigma;
        } else {
            h = node["dist"].as<double>();

            cuboid.sigma = args.sigma;
        };

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
    void readDiscNode(ParticleContainer &particles, const YAML::Node &node, Args &args) const {
        int radius = node["radius"].as<int>();
        Vec3D center = node["center"].as<Vec3<double>>();
        Vec3D velocity = node["velocity"].as<Vec3<double>>();
        double mass = node["mass"].as<double>();
        double brownian_sigma = node["brownian_sigma"].as<double>();
        double spacing = node["dist"].as<double>();
        double epsilon;
        if(node["epsilon"]) {
            epsilon = node["epsilon"].as<double>();
        }else{
            epsilon = args.epsilon;
        }
        double sigma;
        if(node["sigma"]) {
            sigma = node["sigma"].as<double>();
        }else{
            sigma = args.sigma;
        }

        DiscGenerator disc;

        disc.center = center;
        disc.radius = radius;
        disc.spacing = spacing;
        disc.mass = mass;
        disc.initial_velocity = velocity;
        disc.brownian_sigma = brownian_sigma;
        disc.epsilon = epsilon;
        disc.sigma = sigma;


        disc.generate(particles);
    }

    /**
     * @brief Get particle from YAML::Node
     */
    void readBallNode(ParticleContainer &particles, const YAML::Node &node, Args &args) const {
        int radius = node["radius"].as<int>();
        Vec3D center = node["center"].as<Vec3<double>>();
        Vec3D velocity = node["velocity"].as<Vec3<double>>();
        double mass = node["mass"].as<double>();
        double brownian_sigma = node["brownian_sigma"].as<double>();
        double spacing = node["dist"].as<double>();
        double epsilon;
        if(node["epsilon"]) {
             epsilon = node["epsilon"].as<double>();
        }else{
            epsilon = args.epsilon;
        }
        double sigma;
        if(node["sigma"]) {
            sigma = node["sigma"].as<double>();
        }else{
            sigma = args.sigma;
        }

        BallGenerator ball;

        ball.center = center;
        ball.radius = radius;
        ball.spacing = spacing;
        ball.mass = mass;
        ball.initial_velocity = velocity;
        ball.brownian_sigma = brownian_sigma;
        ball.epsilon = epsilon;
        ball.sigma = sigma;



        ball.generate(particles);
    }

    /**
     * @brief Get membrane from YAML::Node
     */
    void readMembraneNode(ParticleContainer &particles, const YAML::Node &node, Args &args) const {
        auto size = node["n"].as<Vec3<int>>();
        Vec3D position = node["position"].as<Vec3<double>>();
        Vec3D velocity = node["velocity"].as<Vec3<double>>();
        double mass = node["mass"].as<double>();
        double spacing = node["dist"].as<double>();
        double stiffness = node["stiffness"] ? node["stiffness"].as<double>() : 300.0;
        double bond_length = node["bond_length"] ? node["bond_length"].as<double>() : spacing;
        
        double epsilon = node["epsilon"] ? node["epsilon"].as<double>() : args.epsilon;
        double sigma = node["sigma"] ? node["sigma"].as<double>() : args.sigma;
        double brownian_sigma = node["brownian_sigma"] ? node["brownian_sigma"].as<double>() : 0.0;

        // Store starting particle index
        //int start_index = particles.particleCount();
        
        // Create 2D grid of particles
        //std::vector<std::vector<int>> membrane_particles(size.x, std::vector<int>(size.y));
        
        // TODO
        MembraneGenerator membrane;

        membrane.center = position;
        membrane.spacing = spacing;
        membrane.mass = mass;
        membrane.initial_velocity = velocity;
        membrane.brownian_sigma = brownian_sigma;
        membrane.epsilon = epsilon;
        membrane.sigma = sigma;
        membrane.width = size.x;
        membrane.height = size.y;
        membrane.stiffness = stiffness;
        membrane.bond_length = bond_length;
        membrane.arguments = args;

        membrane.generate(particles);

        
        // particles.addMembrane(membrane);
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
        const double zUpConstant = unwrap_node<double>(args.gravityFactor, "config", "Z-UP constant");
        const int parallelization_strategy = unwrap_node<int>(args.parallelization_strategy, "config", "parallelization_strategy");

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

        if(boarderXmin == "reflect"){
            args.boarderXmin = 1;
        }else if(boarderXmin == "periodic"){
            args.boarderXmin = 2;
        }else if(boarderXmin == "outflow"){
            args.boarderXmin = 3;
        }else{
            args.boarderXmin = 0;
        }

        if(boarderXmax == "reflect"){
            args.boarderXmax = 1;
        }else if(boarderXmax == "periodic"){
            args.boarderXmax = 2;
        }else if(boarderXmax == "outflow"){
            args.boarderXmax = 3;
        }else{
            args.boarderXmax = 0;
        }

        if(boarderYmin == "reflect"){
            args.boarderYmin = 1;
        }else if(boarderYmin == "periodic"){
            args.boarderYmin = 2;
        }else if(boarderYmin == "outflow"){
            args.boarderYmin = 3;
        }else{
            args.boarderYmin = 0;
        }

        if(boarderYmax == "reflect"){
            args.boarderYmax = 1;
        }else if(boarderYmax == "periodic"){
            args.boarderYmax = 2;
        }else if(boarderYmax == "outflow"){
            args.boarderYmax = 3;
        }else{
            args.boarderYmax = 0;
        }

        if(boarderZmin == "reflect"){
            args.boarderZmin = 1;
        }else if(boarderZmin == "periodic"){
            args.boarderZmin = 2;
        }else if(boarderZmin == "outflow"){
            args.boarderZmin = 3;
        }else{
            args.boarderZmin = 0;
        }

        if(boarderZmax == "reflect"){
            args.boarderZmax = 1;
        }else if(boarderZmax == "periodic"){
            args.boarderZmax = 2;
        }else if(boarderZmax == "outflow"){
            args.boarderZmax = 3;
        }else{
            args.boarderZmax = 0;
        }

        args.gravityFactor = gravityFactor;
        args.zUpConstant = zUpConstant;
        args.parallelization_strategy = parallelization_strategy;
    }

    /**
     * @brief Get particle from YAML::Node
     */
    void readNode(ParticleContainer &particles, const YAML::Node &node, Args &args) const {
        std::string node_type = node["type"] ? (node["type"].as<std::string>()) : "particle";

        if (node_type == "cuboid") {
            return readCuboidNode(particles, node, args);
        }
        
        if (node_type == "ball") {
            return readBallNode(particles, node, args);
        }

        if (node_type == "disc") {
            return readDiscNode(particles, node, args);
        }
        
        if (node_type == "membrane") {
            return readMembraneNode(particles, node, args);
        }

        Particle particle;

        particle.position = node["position"].as<Vec3<double>>();
        particle.velocity = node["velocity"].as<Vec3<double>>();
        particle.mass = node["mass"].as<double>();



        if(node["epsilon"]) {
            particle.epsilon = node["epsilon"].as<double>();
        }else{
            particle.epsilon = args.epsilon;
        }

        if(node["sigma"]) {
            particle.sigma = node["sigma"].as<double>();
        }else{
            particle.sigma = args.sigma;
        }

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
                readNode(particles, *it, args);
            }
        } else if (node.IsMap()) {
            for(YAML::const_iterator it=node.begin();it!=node.end();++it) {
                readNode(particles, it->second, args);
            }
        }

        return true;
    }
};
