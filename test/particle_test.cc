#include <gtest/gtest.h>

#include "../src/Particle.cpp"
#include "../src/ParticleContainer.h"
#include "../src/math/Vec3.h"

/**
 * @brief Simple particle container test to check emplace_back
 * and iterator functionality.
 */
TEST(ParticleContainerTest, BasicAssertions) {
    ParticleContainer particles;

    Particle p1(Vec3D(1.0), Vec3D(4.0), 7, 0);
    Particle p2(Vec3D(2.0), Vec3D(5.0), 8, 0);
    Particle p3(Vec3D(3.0), Vec3D(6.0), 9, 0);

    EXPECT_EQ(particles.size(), 0);

    particles.emplace_back(p1);
    particles.emplace_back(p2);
    particles.emplace_back(p3);

    auto expect_p1 = particles.begin();
    auto expect_p2 = particles.begin() + 1;
    auto expect_p3 = particles.begin() + 2;

    EXPECT_EQ(*expect_p1.getMass(), 7);
    EXPECT_EQ(*expect_p2.getMass(), 8);
    EXPECT_EQ(*expect_p3.getMass(), 9);

    EXPECT_EQ(particles.size(), 3);
}
