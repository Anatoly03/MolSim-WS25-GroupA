/*
 * FileReader.cpp
 *
 *  Created on: 23.02.2010
 *      Author: eckhardw
 */


#include <fstream>
#include <iostream>
#include <sstream>
#include "FileReader.h"
#include "ParticleContainer.h"
#include "math/Vec3.h"

FileReader::FileReader() = default;
FileReader::~FileReader() = default;

void FileReader::readFile(ParticleContainer& particles, char *filename) {
  Vec3D position;
  Vec3D velocity;
  double mass;
  int num_particles = 0;

  std::ifstream input_file(filename);
  std::string tmp_string;

  if (input_file.is_open()) {
    getline(input_file, tmp_string);
    std::cout << "Read line: " << tmp_string << std::endl;

    while (tmp_string.empty() or tmp_string[0] == '#') {
      getline(input_file, tmp_string);
      std::cout << "Read line: " << tmp_string << std::endl;
    }

    std::istringstream numstream(tmp_string);
    numstream >> num_particles;
    std::cout << "Reading " << num_particles << "." << std::endl;
    getline(input_file, tmp_string);
    std::cout << "Read line: " << tmp_string << std::endl;

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
      particles.emplace_back(position, velocity, mass);

      getline(input_file, tmp_string);
      std::cout << "Read line: " << tmp_string << std::endl;
    }
  } else {
    std::cout << "Error: could not open file " << filename << std::endl;
    exit(-1);
  }
}
