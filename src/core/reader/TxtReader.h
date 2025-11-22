
#pragma once

#include <fstream>
#include <iomanip>
#include <sstream>

#include "../ParticleContainer.h"
#include "../math/Vec3.h"
#include "FileReader.h"
#include "spdlog/spdlog.h"

/**
 * @brief Base class for all input readers. This is the entry point for a reader
 * that can read file type and yield a reader subtype, that can read particles.
 */
class TxtReader : public FileReader {
   public:
    /**
     * @note Default constructor without providing particle container is private.
     */
    TxtReader() = default;

    /**
     * @brief Destructor.
     */
    ~TxtReader() = default;

    /**
     * @brief Read custom 'text' file format into particle container.
     */
    virtual void readFile(ParticleContainer &particles, Args &args) override {
        claimFile(args.input_file);
        std::string tmp_string;

        Vec3D position;
        Vec3D velocity;
        double mass;
        int num_particles = 0;

        readLine(tmp_string);

        while (tmp_string.empty() or tmp_string[0] == '#') {
            readLine(tmp_string);
        }

        std::istringstream numstream(tmp_string);
        numstream >> num_particles;

        readLine(tmp_string);
        particles.reserve(num_particles);

        for (int i = 0; i < num_particles; i++) {
            std::istringstream datastream(tmp_string);

            datastream >> position.x;
            datastream >> position.y;
            datastream >> position.z;
            datastream >> velocity.x;
            datastream >> velocity.y;
            datastream >> velocity.z;

            if (datastream.eof()) {
                spdlog::error("read file: eof reached unexpectedly reading from line {}", i);
                exit(-1);
            }
            datastream >> mass;

            particles.add(position, (velocity), mass);

            readLine(tmp_string);
        }
    }
};
