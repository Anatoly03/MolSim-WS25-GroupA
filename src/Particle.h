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

    const Vec3D &getPosition() const;

    const Vec3D &getVelocity() const;

    const Vec3D &getForce() const;

    void setPosition(const Vec3D &position_);

    void setVelocity(const Vec3D &velocity_);

    void setForce(const Vec3D &force_);

    void delayForce();

    const Vec3D &getOldForce() const;

    double getMass() const;

    int getType() const;

    bool operator==(Particle &other);

    std::string toString() const;
};

std::ostream &operator<<(std::ostream &stream, Particle &p);
