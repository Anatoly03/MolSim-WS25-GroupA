
#include "../math/Vec3.h"

/**
 * @brief Represents a force in 3D space.
 */
struct Force {
    Vec3D value;
    Vec3D old_value;

    Force() : value() {}
    Force(const Vec3D &v) : value(v) {}
};

/**
 * @brief Delays the force application by one time frame.
 */
void delayForceSystem(Force &force) {
    force.old_value = force.value;
    force.value = Vec3D(0.0);
}
