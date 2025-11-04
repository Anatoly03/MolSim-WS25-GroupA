/*
 * VTKWriter.h
 *
 *  Created on: 01.03.2010
 *      Author: eckhardw
 */

#pragma once
#ifdef ENABLE_VTK_OUTPUT

#include <vtkSmartPointer.h>
#include <vtkUnstructuredGrid.h>

#include <list>
#include <string>

#include "../ParticleContainer.h"
#include "Particle.h"

namespace outputWriter {

/**
 * This class implements the functionality to generate vtk output from
 * particles using the official VTK library.
 */
class VTKWriter : public Writer {
   public:
    /**
     * @note Default constructor without providing particle container is private.
     */
    XYZWriter() = delete;

    /**
     * @brief Default constructor
     */
    XYZWriter(ParticleContainer &p): Writer(p) {}

    // Delete copy constructor and assignment operator
    VTKWriter(const VTKWriter &) = delete;
    VTKWriter &operator=(const VTKWriter &) = delete;

    /**
     * @brief File Extension is VTK based. (VTU)
     * @override
     */
    const char* getExtension() const override {
        return ".vtu";
    }

    /**
     * Write VTK output of particles.
     * @param particles Particles to add to the output
     * @param filename Output filename
     * @param iteration Current iteration number
     */
    void plotParticles(ParticleContainer &particles, const std::string &filename, int iteration);
};

}
#endif