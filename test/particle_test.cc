#include <gtest/gtest.h>

#include "../src/core/Particle.h"
#include "../src/core/ParticleContainer.h"
#include "../src/core/math/Vec3.h"

/**
 * @brief Simple particle container test to check size
 * and quantity functionality.
 */
TEST(ParticleContainerSizeTest, BasicAssertions) {
    ParticleContainer particles;

    Particle p1(Vec3D(1.0), Vec3D(4.0), 7);
    Particle p2(Vec3D(2.0), Vec3D(5.0), 8);
    Particle p3(Vec3D(3.0), Vec3D(6.0), 9);
    EXPECT_EQ(particles.size(), 0);

    particles.add(p1);
    EXPECT_EQ(particles.size(), 1);

    particles.add(p2);
    EXPECT_EQ(particles.size(), 2);

    particles.add(p3);
    EXPECT_EQ(particles.size(), 3);
}
