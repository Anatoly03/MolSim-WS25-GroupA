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
     * Position of the particle
     */
    Vec3D position;

    /**
     * Velocity of the particle
     */
    Vec3D velocity;

    /**
     * Force effective on this particle
     */
    Vec3D force;

    /**
     * Force which was effective on this particle
     */
    Vec3D old_force;

    /**
     * Mass of this particle
     */
    double mass;

    /**
     * Type of the particle. Use it for whatever you want (e.g. to separate
     * molecules belonging to different bodies, matters, and so on)
     */
    int type;

   public:
    explicit Particle(int type = 0);

    Particle(const Particle &other);

    Particle(
        // for visualization, we need always 3 coordinates
        // -> in case of 2d, we use only the first and the second
        Vec3D pos_arg, Vec3D vel_arg, double mass_arg, int type = 0);

    virtual ~Particle();

    const Vec3D &getPosition() const;

    const Vec3D &getVelocity() const;

    const Vec3D &getForce() const;

    void setPosition(Vec3D &position_);

    void setVelocity(Vec3D &velocity_);

    void setForce(Vec3D &force_);

    void delayForce();

    const Vec3D &getOldForce() const;

    double getMass() const;

    int getType() const;

    bool operator==(Particle &other);

    std::string toString() const;
};

std::ostream &operator<<(std::ostream &stream, Particle &p);
