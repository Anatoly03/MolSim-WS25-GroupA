//
// Created by zhengying zhao on 10.11.25.
//
#pragma once

#include "ParticleContainer.h"
#include "math/Vec3.h"

/**
 * @brief Data structure of parameters for creation of a cuboid of particles.
 */
struct Cuboid {
    /// Low-left-front corner as starting position
    Vec3D position;
    /// Numbers of particles along each axis
    Vec3I size;
    /// Distance between neighboring particles
    double h;
    /// Mass of each particle
    double mass;
    /// Initial Velocity
    Vec3D initial_velocity;
};

/**
 * @brief Adds a cuboid of particles to the container and applies Brownian velocity initialization.
 * @param particles       Reference to the global ParticleContainer.
 * @param cuboid          Holding specific parameters of a cuboid.
 * @param brownian_sigma  Standard deviation (average magnitude) of the Brownian velocity.
 *                        Physically, this corresponds to sqrt(k_B * T / m), but here it is used as a fixed parameter controlling the random velocity magnitude(e.g., 0.1).
 */
void addCuboid(ParticleContainer &particles, const Cuboid &cuboid, double brownian_sigma);

