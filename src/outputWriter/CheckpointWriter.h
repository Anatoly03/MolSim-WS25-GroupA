/* CheckpointWriter.h
 * Write full phase-space checkpoint files.
 */
#pragma once

#include <string>

class ParticleContainer;

namespace outputWriter {

class CheckpointWriter {
  public:
    CheckpointWriter() = default;
    ~CheckpointWriter() = default;

    // write full phase-space to path (overwrites existing file)
    void writeCheckpoint(ParticleContainer &particles, const std::string &path, double time);
};

}  // namespace outputWriter
