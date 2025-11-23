/*
 * Particle.h
 *
 *  Created on: 23.02.2010
 *      Author: eckhardw
 */

#pragma once

#include <array>
#include <string>

#include "math/Vec3.h"

class Particle {
   public:
    /**
     * @brief Position of the particle
     */
    Vec3D position;

    /**
     * @brief Velocity of the particle
     */
    Vec3D velocity;

    /**
     * @brief Force effective on this particle
     */
    Vec3D force;

    /**
     * @brief Force which was effective on this particle
     */
    Vec3D old_force;

    /**
     * @brief Mass of this particle
     */
    double mass;

   public:
    /**
     * @brief Default constructor for Particle.
     */
    Particle(int type = 0) = default;

    /**
     * @brief Copy constructor for Particle.
     */
    Particle(const Particle &other);

    /**
     * @brief Multi-argument constructor for Particle.
     */
    Particle(Vec3D pos_arg, Vec3D vel_arg, double mass_arg);

    /**
     * @brief Particle destructor.
     */
    virtual ~Particle();

    /**
     * @brief Copy constructor for Particle.
     * @note Deep copy.
     */
    Particle clone() const;

    /**
     * @brief Push force to old force and reset current force to zero.
     */
    void delayForce();

    /**
     * @brief Equivalence operation for Particle.
     */
    bool operator==(const Particle &other) const {
        return position == other.position && velocity == other.velocity && force == other.force && type == other.type &&
               mass == other.mass && old_force == other.old_force;
    }

    /**
     * @brief String representation of the Particle.
     */
    std::string toString() const;
};

std::ostream &operator<<(std::ostream &stream, Particle &p);
