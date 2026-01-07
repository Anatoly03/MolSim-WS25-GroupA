/*
 * VTKWriter.cpp
 *
 *  Created on: 01.03.2010
 *      Author: eckhardw
 */
#ifdef ENABLE_VTK_OUTPUT

#include "VTKWriter.h"

#include <vtkCellArray.h>
#include <vtkCellType.h>
#include <vtkDoubleArray.h>
#include <vtkFloatArray.h>
#include <vtkIntArray.h>
#include <vtkPointData.h>
#include <vtkXMLUnstructuredGridWriter.h>

#include <array>
#include <iomanip>
#include <sstream>

#include "../ParticleContainer.h"
#include <string>

namespace outputWriter {

void VTKWriter::plotParticles(ParticleContainer &particles, const std::string &filename, int iteration) {
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

    vtkNew<vtkIntArray> typeArray;
    typeArray->SetName("type");
    typeArray->SetNumberOfComponents(1);

    // Create vertex cells for each particle
    auto cells = vtkSmartPointer<vtkCellArray>::New();
    
    vtkIdType pointId = 0;
    for (const auto &p : particles) {
        points->InsertNextPoint(p.getPosition().asArray().data());
        massArray->InsertNextValue(static_cast<float>(p.getMass()));
        velocityArray->InsertNextTuple(p.getVelocity().asArray().data());
        forceArray->InsertNextTuple(p.getForce().asArray().data());
        typeArray->InsertNextValue(p.getType());
        
        // Create a vertex cell for this particle
        cells->InsertNextCell(1, &pointId);
        pointId++;
    }

    // Set up the grid
    auto grid = vtkSmartPointer<vtkUnstructuredGrid>::New();
    grid->SetPoints(points);
    grid->SetCells(VTK_VERTEX, cells);

    // Add arrays to the grid
    grid->GetPointData()->AddArray(massArray);
    grid->GetPointData()->AddArray(velocityArray);
    grid->GetPointData()->AddArray(forceArray);
    grid->GetPointData()->AddArray(typeArray);

    // Create filename with iteration number
    std::stringstream strstr;
    strstr << filename << "_" << std::setfill('0') << std::setw(4) << iteration << ".vtu";

    // Create writer and set data
    vtkNew<vtkXMLUnstructuredGridWriter> writer;
    writer->SetFileName(strstr.str().c_str());
    writer->SetInputData(grid);
    writer->SetDataModeToAscii();

    // Write the file
    writer->Write();

    (void)filename; (void)iteration; // nothing else to do here
}
}  // namespace outputWriter
#endif
