
#include "../utils/Args.h"
#include "../LinkedCells.h"
#include "../ParticleContainer.h"
#include "Simulation.h"
#include "LinkedCellImplementation.h"

/**
 * @brief places all particles into correct cells.
 */
void LinkedCellImplementation::placeInCells(ParticleContainer &particles) {
    particles.forEach([&](Particle &p) {
        Vec3<int> cellIndex;

        cellIndex.x = (int)(p.position.x) / cellSize.x;
        cellIndex.y = (int)(p.position.y) / cellSize.y;
        cellIndex.z = (int)(p.position.z) / cellSize.z;

        // if (cells[cellIndex] == nullptr) {
        //     cells[cellIndex] = std::vector<Particle>();
        // }

        cells[cellIndex].push_back(p);
    });
}
