#include <iostream>
#include <vector>
#include <string>

#include "gtest/gtest.h"

#include "../my_vector/my_vector.hpp"

TEST(TestMyVector, TestInit) {

    my_vector_t<int> vec;
    std::vector<int> s_vec;

    ASSERT_EQ(vec.size(), s_vec.size());
    // generally there is no point to test for capacity, though
    // during creation this allow to get rid of nasty bugs with zeroing
    ASSERT_EQ(vec.capacity(), s_vec.capacity());
    ASSERT_EQ(vec.data(), s_vec.data());
}

TEST(TestMyVector, TestPushBack) {

    my_vector_t<int> vec;
    std::vector<int> s_vec;
    vec.push_back(2);
    s_vec.push_back(2);

    ASSERT_EQ(s_vec, vec);

    vec.push_back(2);
    s_vec.push_back(2);

    ASSERT_EQ(s_vec, vec);
}

TEST(TestMyVector, TestPopBak) {

    my_vector_t<int> vec{};
    std::vector<int> s_vec{};

    ASSERT_NO_THROW(vec.pop_back());

    // this has ub, size unspecified
    // s_vec.pop_back();

    vec.push_back(2);
    s_vec.push_back(1);
    vec.pop_back();
    s_vec.pop_back();

    ASSERT_EQ(s_vec, vec);
    ASSERT_EQ(vec.size(), s_vec.size());
}

TEST(TestMyVector, TestTakeByIndex) {

    my_vector_t<int> vec{};
    std::vector<int> s_vec{};

    vec.push_back(2);
    vec.push_back(3);
    s_vec.push_back(2);
    s_vec.push_back(3);

    ASSERT_EQ(vec[0], s_vec[0]);
    ASSERT_EQ(vec.at(1), s_vec.at(1));
    ASSERT_THROW(vec.at(3), std::range_error);
}

// note that shrink to fit is tighlty coupled with copy constructor
TEST(TestMyVector, TestShrinkToFit) {
    my_vector_t<int> vec{};

    vec.push_back(2);
    vec.push_back(3);
    vec.push_back(4);

    auto s = vec.size();
    vec.shrink_to_fit();
    ASSERT_EQ(s, vec.capacity());

    vec.reserve(100);
    vec.shrink_to_fit();
    ASSERT_EQ(s, vec.capacity());
}

TEST(TestMyVector, TestSort) {
    my_vector_t<int> vec{7, 3, 7};
    std::vector<int> s_vec{7, 3, 7};

    std::sort(vec.begin(), vec.end());
    std::sort(s_vec.begin(), s_vec.end());
    ASSERT_EQ(vec, s_vec);
}

TEST(TestMyVector, TestMove) {
    my_vector_t<int> s_vec{1, 2};

    my_vector_t<int> other = std::move(s_vec);
    ASSERT_EQ(2, other.size());
    ASSERT_EQ(0, s_vec.size());

    ASSERT_EQ(nullptr, s_vec.data());
}

TEST(TestMyVector, TestCopy) {
    my_vector_t<int> s_vec{1, 2};
    my_vector_t<int> other = s_vec;
    ASSERT_EQ(other, s_vec);

    s_vec = {};
    other = s_vec;
    ASSERT_EQ(other, s_vec);
}

TEST(TestMyVector, TestClear) {
    my_vector_t<int> s_vec{1, 2};
    s_vec.clear();
    ASSERT_EQ(0, s_vec.size());
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}