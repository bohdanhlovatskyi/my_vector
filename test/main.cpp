#include <iostream>
#include <vector>

#include "gtest/gtest.h"

#include "../my_vector/my_vector.hpp"

TEST(TestMyVector, TestPushBack) {

    my_vector_t<int> vec;
    std::vector<int> s_vec;
    vec.push_back(2);
    s_vec.push_back(2);

    print_vector(s_vec);
    std::cout << vec << std::endl;

    ASSERT_EQ(s_vec, vec);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}