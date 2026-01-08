#pragma once

#include "../ParticleContainer.h"
#include "../Particle.h"
#include "../utils/Args.h"
#include "FileReader.h"

#include <fstream>
#include <sstream>
#include <string>

#include "spdlog/spdlog.h"

/**
 * @brief Reader for checkpoint files that restores complete particle state.
 * This reader loads all particle properties including position, velocity,
 * force, old_force, old_position, mass, and p_id to allow complete
 * simulation restart from a saved state.
 */
class CheckpointReader : public FileReader {
   public:
    /**
     * @brief Default constructor
     */
    CheckpointReader() = default;

    /**
     * @brief Destructor.
     */
    ~CheckpointReader() = default;

    /**
     * @brief Read checkpoint file and restore complete particle states.
     * Format expected: x y z vx vy vz m fx fy fz old_fx old_fy old_fz old_x old_y old_z p_id
     * @returns True on success, false on failure.
     */
    virtual bool readFile(ParticleContainer &particles, Args &args) override {
        std::ifstream file(args.input_file);
        
        if (!file.is_open()) {
            spdlog::error("Could not open checkpoint file: {}", args.input_file);
            return false;
        }

        std::string line;
        int lineNum = 0;
        int particleCount = 0;

        while (std::getline(file, line)) {
            lineNum++;
            
            // Skip comments and empty lines
            if (line.empty() || line[0] == '#') {
                continue;
            }

            std::istringstream iss(line);
            Particle particle;
            
            // Read all particle properties
            double x, y, z, vx, vy, vz, m;
            double fx, fy, fz, old_fx, old_fy, old_fz;
            double old_x, old_y, old_z;
            int p_id;

            if (!(iss >> x >> y >> z >> vx >> vy >> vz >> m 
                      >> fx >> fy >> fz >> old_fx >> old_fy >> old_fz 
                      >> old_x >> old_y >> old_z >> p_id)) {
                spdlog::error("Failed to parse checkpoint line {}: {}", lineNum, line);
                return false;
            }

            // Set particle properties
            particle.position = Vec3D(x, y, z);
            particle.velocity = Vec3D(vx, vy, vz);
            particle.mass = m;
            particle.force = Vec3D(fx, fy, fz);
            particle.old_force = Vec3D(old_fx, old_fy, old_fz);
            particle.old_position = Vec3D(old_x, old_y, old_z);
            particle.p_id = p_id;

            particles.add(particle);
            particleCount++;
        }

        file.close();
        spdlog::info("Loaded {} particles from checkpoint file: {}", particleCount, args.input_file);
        
        return particleCount > 0;
    }
};
