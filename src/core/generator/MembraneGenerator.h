
#pragma once

#include "../math/Vec3.h"
#include "../ParticleContainer.h"
#include "Generator.h"

/**
 * @brief Particle generator abstraction.
 */
class MembraneGenerator : public ParticleGenerator {
public:
    /**
     * @brief Position of the cuboid's low-left-front corner.
     */
    Vec3D position;

    /**
     * @brief Numbers of particles along each axis.
     */
    Vec3I size;

    /**
     * @brief Distance between neighboring particles along each axis.
     */
    Vec3D spacing;

    /**
     * @brief Mass of each particle.
     */
    double mass;

    /**
     * @brief Standard deviation (average magnitude) of the Brownian velocity.
     */
    double brownian_sigma;

    /**
     * @brief Initial Velocity of each particle.
     */
    Vec3D initial_velocity;

    /**
     * @brief Standard sigma of the particles in the cuboid.
     */
    double sigma;

    /**
     * @brief Standard epsilon of the particles in the cuboid.
     */
    double epsilon;

    /**
     * @brief safes all id of particle in membrane.
     */
    std::vector<std::vector<int>> membrane;



    /**
     * @brief Generate particles into the given container.
     */
    virtual void generate(ParticleContainer &/*particles*/) override;
};
