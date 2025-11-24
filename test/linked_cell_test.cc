#include <gtest/gtest.h>

#include "../src/core/Particle.h"
#include "../src/core/utils/Args.h"
#include "../src/core/LinkedCells.h"
#include "../src/core/math/Vec3.h"

struct DistinctParticlePair {
   private:
    Particle first;
    Particle second;

   public:
    DistinctParticlePair(Particle a, Particle b): first(a), second(b) {}
    bool has(Particle x) const { return x == first || x == second; }
};

/**
 * @brief Simple particle container test to check size
 * and quantity functionality.
 */
TEST(LinkedCellIndexTest, BasicAssertions) {
    LinkedCells cells(Vec3I(5));

    Particle p1(Vec3D(1.0, 2.0, 3.0), Vec3D(4.0), 7); // in chunk 0,0,0
    Particle p2(Vec3D(2.0, 3.0, 4.0), Vec3D(5.0), 8); // in chunk 0,0,0
    Particle p3(Vec3D(6.0, 7.0, 8.0), Vec3D(6.0), 9); // in chunk 1,1,1
    Particle p4(Vec3D(6.0, 2.0, 2.0), Vec3D(6.0), 9); // in chunk 1,0,0
    Particle p5(Vec3D(11.0, 12.0, 13.0), Vec3D(6.0), 9); // in chunk 2,2,2

    cells.add(p1);
    cells.add(p2);
    cells.add(p3);
    cells.add(p4);

    Args args;
    std::vector<DistinctParticlePair> interactions;

    // set domain to include chunks 0,0,0 to 2,2,2
    cells.setDomainSize(Vec3I(0), Vec3I(2));

    cells.forEachDistinctPair([&interactions](Particle &a, Particle &b) {
        interactions.emplace_back(DistinctParticlePair(a, b));
        std::cout << a.toString() << " <-> " << b.toString() << std::endl;
    });

    bool found_p1_p2 = false;
    bool found_p1_p3 = false;
    bool found_p1_p4 = false;
    bool found_p1_p5 = false;
    bool found_p2_p3 = false;
    bool found_p2_p4 = false;
    bool found_p2_p5 = false;
    bool found_p3_p4 = false;
    bool found_p3_p5 = false;
    bool found_p4_p5 = false;

    for (const auto &pair : interactions) {
        if (pair.has(p1) && pair.has(p2)) found_p1_p2 = true;
        if (pair.has(p1) && pair.has(p3)) found_p1_p3 = true;
        if (pair.has(p1) && pair.has(p4)) found_p1_p4 = true;
        if (pair.has(p1) && pair.has(p5)) found_p1_p5 = true;
        if (pair.has(p2) && pair.has(p3)) found_p2_p3 = true;
        if (pair.has(p2) && pair.has(p4)) found_p2_p4 = true;
        if (pair.has(p2) && pair.has(p5)) found_p2_p5 = true;
        if (pair.has(p3) && pair.has(p4)) found_p3_p4 = true;
        if (pair.has(p3) && pair.has(p5)) found_p3_p5 = true;
        if (pair.has(p4) && pair.has(p5)) found_p4_p5 = true;
    }

    EXPECT_TRUE(found_p1_p2);  // same cell
    EXPECT_TRUE(found_p1_p3);  // neighbouring cell
    EXPECT_TRUE(found_p1_p4);  // neighbouring cell
    EXPECT_FALSE(found_p1_p5); // non-neighbouring cell
    EXPECT_TRUE(found_p2_p3);  // neighbouring cell
    EXPECT_TRUE(found_p2_p4);  // neighbouring cell
    EXPECT_FALSE(found_p2_p5); // non-neighbouring cell
    EXPECT_TRUE(found_p3_p4);  // neighbouring cell over x
    EXPECT_TRUE(found_p3_p5);  // neighbouring cell over diagonal
    EXPECT_FALSE(found_p4_p5); // non-neighbouring cell
}
