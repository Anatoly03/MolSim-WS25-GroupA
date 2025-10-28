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
   private:
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

    /**
     * @brief Type of the particle.
     * @note Use it for whatever you want (e.g. to separate
     * molecules belonging to different bodies, matters, and so on)
     */
    int type;

   public:
    /**
     * @brief Default constructor for Particle.
     */
    explicit Particle(int type = 0);

    /**
     * @brief Copy constructor for Particle.
     */
    Particle(const Particle &other);

    /**
     * @brief Multi-argument constructor for Particle.
     */
    // for visualization, we need always 3 coordinates
    // -> in case of 2d, we use only the first and the second
    Particle(Vec3D pos_arg, Vec3D vel_arg, double mass_arg, int type = 0);

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
     * @brief Get the position of this Particle.
     */
    const Vec3D &getPosition() const;

    /**
     * @brief Get the velocity of this Particle.
     */
    const Vec3D &getVelocity() const;

    /**
     * @brief Get the force effective on this Particle.
     */
    const Vec3D &getForce() const;

    /**
     * @brief Set the position of this Particle.
     */
    void setPosition(const Vec3D &position_);

    /**
     * @brief Set the velocity of this Particle.
     */
    void setVelocity(const Vec3D &velocity_);

    /**
     * @brief Set the force effective on this Particle.
     */
    void setForce(const Vec3D &force_);

    /**
     * @brief Push force to old force and reset current force to zero.
     */
    void delayForce();

    /**
     * @brief Retrieve the old force of the Particle. It is the force
     * delayed by one time step.
     */
    const Vec3D &getOldForce() const;

    /**
     * @brief Retrieve the mass of the Particle.
     */
    double getMass() const;

    /**
     * @brief Reserved.
     */
    int getType() const;

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
