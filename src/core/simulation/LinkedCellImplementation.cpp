
#include "../utils/Args.h"
#include "Simulation.h"
#include "LinkedCellImplementation.h"

/**
 * @brief places all particles into correct cells.
 */
void LinkedCellImplementation::placeInCells(){
    particles.forEach([&](Particle &p) {
        int cx = static_cast<int>(p.position.x / cellSize);
        int cy = static_cast<int>(p.position.y / cellSize);
        int cz = static_cast<int>(p.position.z / cellSize);

        int index = cz * (p.position.x * p.position.y) + cy * p.position.x + cx;
        cells[index].push_back(p);
    });
}
