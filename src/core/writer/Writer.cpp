#include "Writer.h"
#include "../simulation/Simulation.h"

#include <fstream>

/**
 * @brief Plot particles to file.
 */
void Writer::plot(const std::string &filename, int iteration, Simulation &simulation) const {
    std::ofstream file;
    std::string full_filename = getFileName(filename, iteration);

    file.open(full_filename.c_str());
    file << writeMagicHeader(simulation.particleCount());

    simulation.forEachParticle([&file, this](const Particle &p) {
        file << plotSingleParticle(p);
    });

    file.close();
}
