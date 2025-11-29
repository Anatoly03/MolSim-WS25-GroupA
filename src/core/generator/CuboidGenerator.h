
#pragma once

#include "../math/Vec3.h"
#include "../ParticleContainer.h"
#include "Generator.h"

/**
 * @brief Particle generator abstraction.
 */
class CuboidGenerator : public ParticleGenerator {
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
     * @brief Generate particles into the given container.
     */
    virtual void generate(ParticleContainer &/*particles*/) override;
};
