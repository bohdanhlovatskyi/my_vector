#include <iostream>
#include <array>
#include <string>

#include "gtest/gtest.h"

#include "../my_array/my_array.hpp"

TEST(TestMyArray, TestInit) {
    my_array_t<int, 3> arr = {1, 2, 3};
    my_array_t<int, 3> sarr = {1, 2, 3};
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}