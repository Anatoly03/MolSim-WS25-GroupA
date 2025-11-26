/*
 * VTKWriter.h
 *
 *  Created on: 01.03.2010
 *      Author: eckhardw
 */

#pragma once
#ifdef ENABLE_VTK_OUTPUT

#include <vtkCellArray.h>
#include <vtkDoubleArray.h>
#include <vtkFloatArray.h>
#include <vtkIntArray.h>
#include <vtkPointData.h>
#include <vtkSmartPointer.h>
#include <vtkUnstructuredGrid.h>
#include <vtkXMLUnstructuredGridWriter.h>

#include <array>
#include <iomanip>
#include <list>
#include <sstream>
#include <string>

#include "../simulation/Simulation.h"
#include "../Particle.h"
#include "Writer.h"

namespace outputWriter {

/**
 * This class implements the functionality to generate vtk output from
 * particles using the official VTK library.
 */
class VTKWriter : public Writer {
   public:
    /**
     * @brief Default constructor.
     */
    VTKWriter() = default;

    // Delete copy constructor and assignment operator
    VTKWriter(const VTKWriter &) = delete;
    VTKWriter &operator=(const VTKWriter &) = delete;

    /**
     * @brief File Extension is VTK based. (VTU)
     * @override
     */
    virtual const char *getExtension() const override { return ".vtu"; }

    // TODO refactor: use writeMagicHeader
    // TODO refactor: use plotSingleParticle

    /**
     * @brief Plot particles to file, returns stream.
     */
    virtual void plot(const std::string &filename, int iteration,  Simulation &simulation) const override {
        // Initialize points
        auto points = vtkSmartPointer<vtkPoints>::New();

        // Create and configure data arrays
        vtkNew<vtkFloatArray> massArray;
        massArray->SetName("mass");
        massArray->SetNumberOfComponents(1);

        vtkNew<vtkFloatArray> velocityArray;
        velocityArray->SetName("velocity");
        velocityArray->SetNumberOfComponents(3);

        vtkNew<vtkFloatArray> forceArray;
        forceArray->SetName("force");
        forceArray->SetNumberOfComponents(3);

        // vtkNew<vtkIntArray> typeArray;
        // typeArray->SetName("type");
        // typeArray->SetNumberOfComponents(1);

        simulation.forEachParticle([&points,&massArray,&velocityArray,&forceArray,this](const Particle &p) {
            points->InsertNextPoint(p.position.asArray().data());
            massArray->InsertNextValue(static_cast<float>(p.mass));
            velocityArray->InsertNextTuple(p.velocity.asArray().data());
            forceArray->InsertNextTuple(p.force.asArray().data());
            //typeArray->InsertNextValue(p.getType());
        });

        // Set up the grid

        auto grid = vtkSmartPointer<vtkUnstructuredGrid>::New();
        grid->SetPoints(points);

        // Add arrays to the grid
        grid->GetPointData()->AddArray(massArray);
        grid->GetPointData()->AddArray(velocityArray);
        grid->GetPointData()->AddArray(forceArray);
        //grid->GetPointData()->AddArray(typeArray);

        // Create filename with iteration number
        std::string full_filename = getFileName(filename, iteration);

        // Create writer and set data
        vtkNew<vtkXMLUnstructuredGridWriter> writer;
        writer->SetFileName(full_filename.c_str());
        writer->SetInputData(grid);
        writer->SetDataModeToAscii();

        // Write the file
        writer->Write();
    }
};

}  // namespace outputWriter
#endif