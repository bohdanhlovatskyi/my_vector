#include <iostream>

#include "gtest/gtest.h"

#include "../my_vector/my_vector.hpp"

TEST(TestLinkedList, TestFind) {
    std::cout << "Hello" << std::endl;
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}