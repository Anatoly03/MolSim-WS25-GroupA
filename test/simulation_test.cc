#include <gtest/gtest.h>

#include "../src/core/Particle.h"
#include "../src/core/ParticleContainer.h"
#include "../src/core/simulation/Simulation.h"
#include "../src/core/math/Vec3.h"

/**
 * @brief Simple correctness check of position not updating for
 * zero delta.
 */
TEST(SimulationPositionZeroDeltaTest, BasicAssertions) {
    ParticleContainer particles;
    Args args;
    args.delta_t = 0.0;

    Simulation simulation(particles, args);
    Particle part(Vec3D(1.0, 2.0, 3.0), Vec3D(1.0), 1.0);

    simulation.calculateSinglePosition(part, args.delta_t);
    EXPECT_EQ(part.position, Vec3D(1.0, 2.0, 3.0));
}

/**
 * @brief Simple correctness check of position updating for
 * one delta.
 */
TEST(SimulationPositionTest, BasicAssertions) {
    ParticleContainer particles;
    Args args;
    args.delta_t = 1.0;

    Simulation simulation(particles, args);
    Particle part(Vec3D(1.0, 2.0, 3.0), Vec3D(1.0), 1.0);
    part.force = Vec3D(0.0);

    simulation.calculateSinglePosition(part, args.delta_t);
    EXPECT_EQ(part.position, Vec3D(2.0, 3.0, 4.0));

    simulation.calculateSinglePosition(part, args.delta_t);
    EXPECT_EQ(part.position, Vec3D(3.0, 4.0, 5.0));
}

/**
 * @brief Not-Simple correctness check that two nearby particles attract each
 * other. Do not edit this test.
 */
/**
TEST(DirectSumAttractionTest, BasicAssertions) {
    Args args;
    args.delta_t = 0.05;

    ParticleContainer particles;
    // if distance is too large particles may not attract each other
    Particle part1(Vec3D(1.0, 0.0, 0.0), Vec3D(1.0), 0.1);
    Particle part2(Vec3D(3.0, 0.0, 0.0), Vec3D(1.0), 0.1);
    particles.add(part1);
    particles.add(part2);

    Simulation simulation(particles, args);
    double distance = (part1.position - part2.position).length();

    // pretick as position only updates after force and velocity propagates
    // (i think this is how physics work, but even if it's done on one tick i'll better be safe)
    // tick 1: force is calculates
    // tick 2: velocity is applied
    // tick 3: position is updated
    for (int tick = 0; tick < 3; tick++) {
        simulation.tick();
    }

    EXPECT_EQ(simulation.particleCount(), 2);

    // if tick is set to too large particles might start repelling each other.
    for (int tick = 0; tick < 12; tick++) {
        simulation.tick();
        int pairs = 0;

        simulation.forEachDistinctParticlePair([&pairs, &distance, &tick](Particle& p1, Particle& p2) {
            double new_distance = (p1.position - p2.position).length();

            if (distance <= new_distance) {
                // should be unreachable: particles did not attract each other
                FAIL() << "particles did not attract each other: at tick " << tick << ", distance was " << distance << " and is now " << new_distance;
            }
            
            pairs++;
            distance = new_distance;
        });

        EXPECT_EQ(pairs, 1);
    }
}
*/
