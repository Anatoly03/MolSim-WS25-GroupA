
#include <functional>

#include "../utils/Args.h"
#include "../Particle.h"

/**
 * @brief Abstraction for a force calculation lambda between two particles.
 */
typedef const std::function<void(const Args& args, Particle &, Particle &)> force_calculation_system;

/**
 * @brief Gravitational force calculation system.
 */
force_calculation_system gravity_system;

/**
 * @brief Lennard-Jones force calculation system.
 */
force_calculation_system lennard_jones_system;
