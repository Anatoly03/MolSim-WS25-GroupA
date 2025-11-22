#include <gtest/gtest.h>

#include "../src/core/Particle.h"
#include "../src/core/ParticleContainer.h"
#include "../src/core/math/Vec3.h"

/**
 * @brief Simple particle container test to check add
 * and iterator functionality.
 */
TEST(ParticleContainerTest, BasicAssertions) {
    ParticleContainer particles;

    Particle p1(Vec3D(1.0), Vec3D(4.0), 7, 0);
    Particle p2(Vec3D(2.0), Vec3D(5.0), 8, 0);
    Particle p3(Vec3D(3.0), Vec3D(6.0), 9, 0);

    EXPECT_EQ(particles.size(), 0);

    particles.add(p1);
    particles.add(p2);
    particles.add(p3);

    // TODO fix the c++ syntax below

    // auto expect_p1 = particles.begin();
    // auto expect_p2 = particles.begin() + 1;
    // auto expect_p3 = particles.begin() + 2;

    // EXPECT_EQ(*expect_p1.getMass(), 7);
    // EXPECT_EQ(*expect_p2.getMass(), 8);
    // EXPECT_EQ(*expect_p3.getMass(), 9);

    EXPECT_EQ(particles.size(), 3);
}
