#include "optimizationtools/containers/space_efficient_array.hpp"

#include <gtest/gtest.h>

using namespace optimizationtools;

TEST(SpaceEfficientArray, Test1)
{
    optimizationtools::SpaceEfficientArrayFactory array_factory(10, 120000);
    SpaceEfficientArray array = array_factory.create_array();

    array_factory.set(array, 0, 118387);
    EXPECT_EQ(array_factory.get(array, 0), 118387);
    EXPECT_EQ(array_factory.get(array, 1), 0);
    EXPECT_EQ(array_factory.get(array, 2), 0);
    EXPECT_EQ(array_factory.get(array, 3), 0);
    EXPECT_EQ(array_factory.get(array, 4), 0);

    array_factory.set(array, 1, 101475);
    EXPECT_EQ(array_factory.get(array, 0), 118387);
    EXPECT_EQ(array_factory.get(array, 1), 101475);
    EXPECT_EQ(array_factory.get(array, 2), 0);
    EXPECT_EQ(array_factory.get(array, 3), 0);
    EXPECT_EQ(array_factory.get(array, 4), 0);

    array_factory.set(array, 3, 93623);
    EXPECT_EQ(array_factory.get(array, 0), 118387);
    EXPECT_EQ(array_factory.get(array, 1), 101475);
    EXPECT_EQ(array_factory.get(array, 2), 0);
    EXPECT_EQ(array_factory.get(array, 3), 93623);
    EXPECT_EQ(array_factory.get(array, 4), 0);

    delete[] array;
}
