/*
 * FileReader.h
 *
 *  Created on: 23.02.2010
 *      Author: eckhardw
 */

#pragma once
#include "ParticleContainer.h"

class FileReader {
   public:
    FileReader();
    virtual ~FileReader();

    // readFile: reads particles and optional checkpoint time. If out_time != nullptr
    // and the file contains a "#time <t>" header, *out_time is set to that value.
    void readFile(ParticleContainer &particles, char *filename, double *out_time = nullptr);
};
