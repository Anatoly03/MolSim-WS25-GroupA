/* CheckpointWriter.cpp
 * Write full phase-space checkpoint files used for restart.
 */

#include "CheckpointWriter.h"

#include <fstream>
#include <iomanip>
#include <iostream>

#include "ParticleContainer.h"

namespace outputWriter {

void CheckpointWriter::writeCheckpoint(ParticleContainer &particles, const std::string &path, double time) {
    std::ofstream out(path, std::ios::out | std::ios::trunc);
    if (!out.is_open()) {
        std::cerr << "error: could not open checkpoint file " << path << " for writing" << std::endl;
        return;
    }

    out << "#CHECKPOINT v1\n";
    out << "#time " << std::fixed << std::setprecision(8) << time << "\n";
    out << particles.size() << "\n";

    particles.forEach([&out](Particle &p) {
        const auto &pos = p.getPosition();
        const auto &vel = p.getVelocity();
        const auto &f = p.getForce();
        const auto &of = p.getOldForce();

        out << std::setprecision(12) << pos.x << " " << pos.y << " " << pos.z << " ";
        out << vel.x << " " << vel.y << " " << vel.z << " ";
        out << p.getMass() << " ";
        out << p.getType() << " ";
        out << f.x << " " << f.y << " " << f.z << " ";
        out << of.x << " " << of.y << " " << of.z << "\n";
    });

    out.close();
    std::cout << "Checkpoint written to " << path << " (" << particles.size() << " particles)" << std::endl;
}

}  // namespace outputWriter
