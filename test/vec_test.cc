#include <gtest/gtest.h>

#include "../src/core/math/Vec3.h"
#include "../src/core/math/Vec3Iter.h"

/**
 * @brief Simple vector comparison test.
 */
TEST(VecEqTest, BasicAssertions) {
    Vec3D a(1.0, 2.0, 3.0);
    Vec3D b(4.0, 5.0, 6.0);

    // Test Equality
    EXPECT_EQ(a, Vec3D(1.0, 2.0, 3.0));
    EXPECT_NE(a, b);

    // Test Constructor Equality
    EXPECT_EQ(Vec3(1), Vec3(1, 1, 1));
}

/**
 * @brief Simple vector arithmetics test.
 */
TEST(VecScalarTest, BasicAssertions) {
    Vec3D a(1.0, 2.0, 3.0);
    Vec3D b(4.0, 5.0, 6.0);

    // Test Scalar Arithmetics
    Vec3D c = a * 2.0;
    EXPECT_DOUBLE_EQ(c.x, 2.0);
    EXPECT_DOUBLE_EQ(c.y, 4.0);
    EXPECT_DOUBLE_EQ(c.z, 6.0);

    // Test Vec Arithmetics
    EXPECT_EQ(b - a, Vec3D(3.0));
    EXPECT_EQ(a + b, Vec3D(5.0, 7.0, 9.0));
}

/**
 * @brief Simple vector dot product tests.
 */
TEST(VecDotTest, BasicAssertions) {
    Vec3D a(1.0, 2.0, 3.0);
    Vec3D b(4.0, 5.0, 6.0);

    // Test dot product
    EXPECT_DOUBLE_EQ(a.dot(a), 14.0);
    EXPECT_DOUBLE_EQ(a.dot(b), 32.0);
}

/**
 * @brief Simple YAML conversion tests.
 */
TEST(VecYAMLTest, BasicAssertions) {
    YAML::Node node = YAML::Load("[1.0, 2.0, 3.0]");
    Vec3D v = node.as<Vec3D>();

    EXPECT_DOUBLE_EQ(v.x, 1.0);
    EXPECT_DOUBLE_EQ(v.y, 2.0);
    EXPECT_DOUBLE_EQ(v.z, 3.0);

    Vec3D v2(1.0, 2.0, 3.0);
    YAML::Emitter out;
    out << YAML::convert<Vec3D>::encode(v2);
    node = YAML::Load(out.c_str());
    Vec3D v3 = node.as<Vec3D>();

    EXPECT_EQ(v2, v3);
}

/**
 * @brief Simple hashmap indexation by vector test.
 */
TEST(VecHashMapTest, BasicAssertions) {
    std::map<Vec3I, int> vecMap;
    
    Vec3I v1(1, 2, 3);
    Vec3I v2(4, 5, 6);
    Vec3I v3(v1.x, v1.y, v1.z); // equal to v1 in value

    vecMap[v1] = 1;
    vecMap[v2] = 2;

    EXPECT_EQ(vecMap[v1], 1);
    EXPECT_EQ(vecMap[v2], 2);
    EXPECT_EQ(vecMap[v3], 1);
}

/**
 * @brief Test vector iterator.
 */
TEST(VecIterTest, BasicAssertions) {
    Vec3Iter iter(1, 2, 3);

    // init
    EXPECT_TRUE(iter.hasNext());
    Vec3<int> v = *iter;
    EXPECT_EQ(v, Vec3I(0, 0, 0));

    // loops
    EXPECT_TRUE(iter.hasNext());
    ++iter;
    v = *iter;
    EXPECT_EQ(v, Vec3I(0, 0, 1));

    EXPECT_TRUE(iter.hasNext());
    ++iter;
    v = *iter;
    EXPECT_EQ(v, Vec3I(0, 1, 0));

    EXPECT_TRUE(iter.hasNext());
    ++iter;
    v = *iter;
    EXPECT_EQ(v, Vec3I(0, 1, 1));

    EXPECT_TRUE(iter.hasNext());
    ++iter;
    v = *iter;
    EXPECT_EQ(v, Vec3I(1, 0, 0));

    EXPECT_TRUE(iter.hasNext());
    ++iter;
    v = *iter;
    EXPECT_EQ(v, Vec3I(1, 0, 1));

    EXPECT_TRUE(iter.hasNext());
    ++iter;
    v = *iter;
    EXPECT_EQ(v, Vec3I(1, 1, 0));

    EXPECT_TRUE(iter.hasNext());
    ++iter;
    v = *iter;
    EXPECT_EQ(v, Vec3I(1, 1, 1));

    EXPECT_FALSE(iter.hasNext());
}
