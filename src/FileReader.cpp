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
#include "spdlog/spdlog.h"

FileReader::FileReader() = default;
FileReader::~FileReader() = default;

void FileReader::readFile(ParticleContainer &particles, char *filename) {
    Vec3D position;
    Vec3D velocity;
    double mass;
    int num_particles = 0;

    std::ifstream input_file(filename);
    std::string tmp_string;

    if (input_file.is_open()) {
        getline(input_file, tmp_string);
        spdlog::debug("Read line: {}", tmp_string);

        while (tmp_string.empty() or tmp_string[0] == '#') {
            getline(input_file, tmp_string);
            spdlog::debug("Read line: {}", tmp_string);
        }

        std::istringstream numstream(tmp_string);
        numstream >> num_particles;
        spdlog::info("Reading {}.", num_particles);
        getline(input_file, tmp_string);
        spdlog::debug("Read line: {}", tmp_string);
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
                spdlog::error("error: read file: eof reached unexpectedly reading from line {}", i);
                exit(-1);
            }
            datastream >> mass;
            
            particles.emplace_back(position, (velocity), mass);

            getline(input_file, tmp_string);
            spdlog::debug("read line: {}", tmp_string);
        }
    } else {
        spdlog::error("error: could not open file {}", filename);
        exit(-1);
    }
}
