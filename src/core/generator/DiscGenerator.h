
#pragma once

#include "../math/Vec3.h"
#include "../ParticleContainer.h"
#include "Generator.h"

/**
 * @brief Particle generator abstraction.
 */
class DiscGenerator : public ParticleGenerator {
   public:
    /**
     * @brief Focus point/ center of disc.
     */
    Vec3D center;

    /**
     * @brief Radius of the disc, specifically the number of molecules along the
     * axis lines of the disc.
     */
    int radius;

    /**
     * @brief Distance between neighboring particles along each axis.
     */
    double spacing;
    
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
     * @brief Standard epsilon of the particles in the cuboid.
     */
    double epsilon;

   protected:
    /**
     * Constant, amount of dimensions.
     */
    virtual int dimensions() const {
        return 2;
    };

    /**
     * @brief Generate particles into the given container.
     */
   public:
    virtual void generate(ParticleContainer &/*particles*/) override;
};
