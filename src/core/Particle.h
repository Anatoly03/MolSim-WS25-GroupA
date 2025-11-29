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
    Vec3D position = Vec3D(0.0);

    /**
     * @brief Position of the particle
     */
    Vec3D old_position = Vec3D(0.0);

    /**
     * @brief Velocity of the particle
     */
    Vec3D velocity = Vec3D(0.0);

    /**
     * @brief Force effective on this particle
     */
    Vec3D force = Vec3D(0.0);

    /**
     * @brief Force which was effective on this particle
     */
    Vec3D old_force = Vec3D(0.0);

    /**
     * @brief Mass of this particle
     */
    double mass = 0.0;

    /**
     * @brief Unique particle ID
     */
    // NOLINTNEXTLINE unused-variable
    int p_id = 0;

   public:
    /**
     * @brief Default constructor for Particle.
     */
    Particle();

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
     * @brief Push position to old position. Does not modify current position.
     */
    void delayPosition();

    /**
     * @brief Push force to old force and reset current force to zero.
     */
    void delayForce();

    /**
     * @brief Equivalence operation for Particle.
     */
    bool operator==(const Particle &other) const {
        return (position == other.position) && (velocity == other.velocity) && (mass == other.mass)
            && (force == other.force) && (old_force == other.old_force);
    }

    /**
     * @brief String representation of the Particle.
     */
    std::string toString() const;
};

std::ostream &operator<<(std::ostream &stream, Particle &p);
