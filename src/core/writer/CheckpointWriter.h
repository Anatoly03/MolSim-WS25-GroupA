#pragma once

#include <fstream>
#include <iomanip>
#include <sstream>

#include "../Particle.h"
#include "../simulation/Simulation.h"
#include "Writer.h"
#include "spdlog/spdlog.h"

/**
 * @brief Writer for checkpoint files that saves complete particle state.
 * This writer saves all particle properties including position, velocity,
 * force, old_force, old_position, mass, and p_id to allow complete
 * simulation restart.
 */
class CheckpointWriter : public Writer {
   public:
    /**
     * @brief Default constructor
     */
    CheckpointWriter() = default;

    /**
     * @brief Destructor.
     */
    virtual ~CheckpointWriter() = default;

    /**
     * @brief Standard file extension for checkpoint files.
     */
    virtual const char *getExtension() const override { return ".chkpt"; }

   protected:
    /**
     * @brief Write checkpoint header with metadata.
     */
    virtual const std::string writeMagicHeader(int particleCount = 0) const override {
        std::stringstream strstr;
        strstr << "# MolSim Checkpoint File\n";
        strstr << "# Format: x y z vx vy vz m fx fy fz old_fx old_fy old_fz old_x old_y old_z p_id\n";
        strstr << "# Particle count: " << particleCount << "\n";
        return strstr.str();
    }

    /**
     * @brief Write complete state of a single particle.
     * Format: position(3) velocity(3) mass(1) force(3) old_force(3) old_position(3) p_id(1)
     */
    virtual const std::string plotSingleParticle(const Particle &part) const override {
        std::stringstream strstr;
        strstr << std::setprecision(17); // High precision for exact restart
        
        // Position
        strstr << part.position.x << " " << part.position.y << " " << part.position.z << " ";
        
        // Velocity
        strstr << part.velocity.x << " " << part.velocity.y << " " << part.velocity.z << " ";
        
        // Mass
        strstr << part.mass << " ";
        
        // Current force
        strstr << part.force.x << " " << part.force.y << " " << part.force.z << " ";
        
        // Old force
        strstr << part.old_force.x << " " << part.old_force.y << " " << part.old_force.z << " ";
        
        // Old position
        strstr << part.old_position.x << " " << part.old_position.y << " " << part.old_position.z << " ";
        
        // Particle ID
        strstr << part.p_id << "\n";
        
        return strstr.str();
    }

   public:
    /**
     * @brief Plot particles to checkpoint file without iteration number.
     * This is meant for saving simulation state at specific times.
     */
    void plotCheckpoint(const std::string &filename, Simulation &simulation) const {
        std::ofstream file(filename);
        
        if (!file.is_open()) {
            spdlog::error("Failed to open checkpoint file: {}", filename);
            return;
        }

        int count = simulation.particleCount();
        file << writeMagicHeader(count);

        simulation.forEachParticle([&](Particle &particle) {
            file << plotSingleParticle(particle);
        });

        file.close();
        spdlog::info("Checkpoint saved to: {}", filename);
    }
};
