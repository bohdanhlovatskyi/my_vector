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

TEST(TestMyVector, TestBack) {
    my_vector_t<int> s_vec{1, 2};

    while (s_vec.back() != 0) {
        s_vec.push_back(s_vec.back() -1 );
    }

    my_vector_t<int> a{1, 2, 1, 0};
    ASSERT_EQ(a, s_vec);
}

TEST(TestMyVector, TestResize) {
    my_vector_t<int> v{1, 2};
    ASSERT_EQ(2, v.size());
    v.resize(5);
    ASSERT_EQ(5, v.size());
    ASSERT_EQ(0, v.back());

    v.resize(2);
    ASSERT_EQ(2, v.size());
    ASSERT_EQ(2, v.back());

    v.resize(3, 3);
    ASSERT_EQ(3, v.size());
    ASSERT_EQ(3, v.back());
}

TEST(TestMyVector, TestInsert) {
    my_vector_t<int> v{2, 3};
    v.insert(v.begin(), 1);
    v.insert(v.begin() + 1, 3);
    ASSERT_EQ(4, v.size());
    my_vector_t<int> v1{1, 3, 2, 3};
    ASSERT_EQ(v1, v);
}

TEST(TestMyVector, TestMultiInsert) {
    my_vector_t<int> v{2, 3};
    my_vector_t<int> v2{3, 4};
    v.insert(v.begin() + 1, v2.begin(), v2.end());
    ASSERT_EQ(4, v.size());
    my_vector_t<int> v1{2, 3, 4, 3};
    ASSERT_EQ(v1, v);
}

TEST(TestMyVector, TestMultiInsertAtBeginning) {
    my_vector_t<int> v{2, 3};
    my_vector_t<int> v2{3, 4};
    v.insert(v.begin(), v2.begin(), v2.end());
    ASSERT_EQ(4, v.size());
    my_vector_t<int> v1{3, 4, 2, 3};
    ASSERT_EQ(v1, v);
}

TEST(TestMyVector, TestMultiInsertAtEnd) {
    my_vector_t<int> v{2, 3};
    my_vector_t<int> v2{3, 4};
    v.insert(v.end(), v2.begin(), v2.end());
    ASSERT_EQ(4, v.size());
    my_vector_t<int> v1{2, 3, 3, 4};
    ASSERT_EQ(v1, v);
}

TEST(TestMyVector, TestMultiErase) {
    my_vector_t<int> v{2, 3, 4, 5, 6};
    v.erase(v.begin() + 1, v.begin() + 3);
    ASSERT_EQ(3, v.size());
    my_vector_t<int> v1{2, 5, 6};
    ASSERT_EQ(v1, v);
}

TEST(TestMyVector, TestMultiEraseALl) {
    my_vector_t<int> v{2, 3, 4, 5, 6};
    v.erase(v.begin(), v.end());
    ASSERT_EQ(0, v.size());
    my_vector_t<int> v1{};
    ASSERT_EQ(v1, v);
}

TEST(TestMyVector, TestErase) {
    my_vector_t<int> v{2, 3, 4, 5, 6};
    v.erase(v.begin());
    ASSERT_EQ(4, v.size());
    my_vector_t<int> v1{3, 4, 5, 6};
    ASSERT_EQ(v1, v);

    v.erase(v.end());
    ASSERT_EQ(3, v.size());
    v1 = {3, 4, 5};
    ASSERT_EQ(v1, v);

    v.erase(v.begin() + 1);
    ASSERT_EQ(2, v.size());
    v1 = {3, 5};
    ASSERT_EQ(v1, v);
}

TEST(TestMyVector, TestComparissons) {
    my_vector_t<int> v{2, 3, 4, 5, 6};
    my_vector_t<int> v1{3, 4, 5, 6};
    ASSERT_TRUE(v1 > v);
    ASSERT_TRUE(v < v1);
    ASSERT_TRUE(!(v1 < v));
    ASSERT_TRUE(!(v1 == v));
    ASSERT_TRUE(v1 != v);
    ASSERT_TRUE(v1 >= v);
    ASSERT_TRUE(!(v1 <= v));

    v = {};
    v1 = {1};
    ASSERT_TRUE(v1 > v);
    ASSERT_TRUE(v < v1);
    ASSERT_TRUE(!(v1 < v));
    ASSERT_TRUE(!(v1 == v));
    ASSERT_TRUE(v1 != v);
    ASSERT_TRUE(v1 >= v);
    ASSERT_TRUE(!(v1 <= v));
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}