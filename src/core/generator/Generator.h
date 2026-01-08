
#pragma once

#include "../ParticleContainer.h"

/**
 * @brief Particle generator abstraction.
 */
class ParticleGenerator {
   public:
    /**
     * @brief Default constructor.
     */
    ParticleGenerator() = default;
   
    /**
     * @brief Destructor.
     */
    virtual ~ParticleGenerator() = default;
   
    /**
     * @brief Generate particles into the given container.
     */
    virtual void generate(ParticleContainer &/*particles*/) {}
};
