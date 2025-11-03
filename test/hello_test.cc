// code cited from https://google.github.io/googletest/quickstart-cmake.html

#include <gtest/gtest.h>

#include "../src/Particle.h"
#include "../src/ParticleContainer.h"
#include "../src/math/Vec3.h"


// Demonstrate some basic assertions.
TEST(HelloTest, BasicAssertions) {
  // Expect two strings not to be equal.
  EXPECT_STRNE("hello", "world");
  // Expect equality.
  EXPECT_EQ(7 * 6, 42);
}

TEST(ParticalContainerTestEmplace_back, BasicAssertions) {
    ParticleContainer particles;
    Vec3<double> position1;
    Vec3<double> position2(1.0);
    Vec3<double> velocity1;
    Vec3<double> velocity2(1.0);
    Particle p1(position1,velocity1,2,0);
    Particle p2(position2,velocity2,2,0);
    particles.emplace_back(p1);
    particles.emplace_back(p2);
    int counter=0;
    for (auto &particle : particles) {
        counter++;
        if(counter==1)
        {
            EXPECT_EQ(particle, p1);
        }else if(counter==2){
            EXPECT_EQ(particle, p2);
        }else{
            EXPECT_EQ(7 * 6, 43);
        }
     }
}
