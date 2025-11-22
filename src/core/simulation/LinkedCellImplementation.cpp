
#include "../utils/Args.h"
#include "Simulation.h"
#include "LinkedCellImplementation.h"

/**
 * @brief places all particles into correct cells.
 */
void LinkedCellImplementation::placeInCells(){
    particles.forEach([&](Particle &p) {
        int cx = static_cast<int>(p.getPosition().x / cellSize);
        int cy = static_cast<int>(p.getPosition().y / cellSize);
        int cz = static_cast<int>(p.getPosition().z / cellSize);

        int index = cz * (p.getPosition().x * p.getPosition().y) + cy * p.getPosition().x + cx;
        cells[index].push_back(p);
    });
}
