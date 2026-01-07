/*
 * FileReader.cpp
 *
 *  Created on: 23.02.2010
 *      Author: eckhardw
 */

#include "FileReader.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include "ParticleContainer.h"
#include "math/Vec3.h"

FileReader::FileReader() = default;
FileReader::~FileReader() = default;

void FileReader::readFile(ParticleContainer &particles, char *filename, double *out_time) {
    Vec3D position;
    Vec3D velocity;
    double mass;
    int num_particles = 0;

    std::ifstream input_file(filename);
    std::string tmp_string;

    if (input_file.is_open()) {
        getline(input_file, tmp_string);
        std::cout << "Read line: " << tmp_string << std::endl;

        // if the first non-empty/comment line starts with #CHECKPOINT, try to parse a #time line next
        while (tmp_string.empty() || tmp_string[0] == '#') {
            // if this is a #time header and out_time is provided, parse the time
            if (out_time != nullptr) {
                std::istringstream header(tmp_string);
                std::string tag;
                header >> tag;
                if (tag == "#time") {
                    double tval = 0.0;
                    if (header >> tval) {
                        *out_time = tval;
                        std::cout << "Parsed checkpoint time: " << *out_time << std::endl;
                    }
                }
            }

            getline(input_file, tmp_string);
            std::cout << "Read line: " << tmp_string << std::endl;
        }

        std::istringstream numstream(tmp_string);
        numstream >> num_particles;
        std::cout << "Reading " << num_particles << "." << std::endl;
        getline(input_file, tmp_string);
        std::cout << "Read line: " << tmp_string << std::endl;
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
                std::cout << "Error reading file: eof reached unexpectedly reading from line " << i << std::endl;
                exit(-1);
            }
            datastream >> mass;

            // default values in case it's a legacy/simple input file
            int type = 0;
            Vec3D force{0.0, 0.0, 0.0};
            Vec3D old_force{0.0, 0.0, 0.0};

            // try to read additional fields if present (type, force, old_force)
            if (datastream >> type) {
                // attempt to read current force
                if (!(datastream >> force.x >> force.y >> force.z)) {
                    // reset to zero if incomplete
                    force = Vec3D(0.0);
                }

                // attempt to read old force
                if (!(datastream >> old_force.x >> old_force.y >> old_force.z)) {
                    old_force = Vec3D(0.0);
                }
            }

            // emplace directly and then set forces to avoid an extra deprecated copy
            particles.emplace_back(position, (velocity), mass, type);
            // set the forces on the emplaced particle
            particles.back().setForce(force);
            particles.back().setOldForce(old_force);

            getline(input_file, tmp_string);
            std::cout << "Read line: " << tmp_string << std::endl;
        }
    } else {
        std::cout << "error: could not open file " << filename << std::endl;
        exit(-1);
    }
}
