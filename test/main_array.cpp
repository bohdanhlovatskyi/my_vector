#include <iostream>
#include <array>
#include <string>

#include "gtest/gtest.h"

#include "../my_array/my_array.hpp"

TEST(TestMyArray, TestInit) {
    my_array_t<int, 3> arr = {1, 2, 3};
    
    ASSERT_EQ(3, arr.size());
    ASSERT_EQ(1, arr[0]);
    ASSERT_EQ(2, arr[1]);
    ASSERT_EQ(3, arr[2]);
}


TEST(TestMyArray, TestFill) {
    my_array_t<int, 3> arr;
    arr.fill(0);
    for (const auto& elm : arr) {
        ASSERT_EQ(0, elm);
    }

    my_array_t<int, 3> arr2{3, 4, 3};
    arr2.fill(1);
    for (const auto& elm : arr2) {
        ASSERT_EQ(1, elm);
    }
}

TEST(TestMyArray, TestAt) {
    using namespace std::string_literals;
    my_array_t<std::string, 2> data = { "love", "cpp" };
    data.at(0) = "hate"s;
    ASSERT_EQ("hate"s, data[0]);

    ASSERT_THROW(data.at(3), std::out_of_range);
}

// I guess this one is the only interesting there, as it should preserve
// all the pointers and references
TEST(TestMyArray, TestSwap) {
    // does not make iterators point at another array
    my_array_t<int, 2> arr1{1, 2};
    my_array_t<int, 2> arr2{3, 4};

    auto it1 = arr1.begin();
    auto it2 = arr2.begin();
    int& ref1 = arr1[1];
    int& ref2 = arr2[1];

    arr1.swap(arr2);

    ASSERT_EQ(3, arr1[0]);
    ASSERT_EQ(4, arr1[1]);
    ASSERT_EQ(1, arr2[0]);
    ASSERT_EQ(2, arr2[1]);

    ASSERT_EQ(ref1, 4);
    ASSERT_EQ(ref2, 2);

    ASSERT_EQ(*it1, 3);
    ASSERT_EQ(*it2, 1);

}

TEST(TestMyArray, TestToArray) {
    // does not make iterators point at another array
    my_array_t<char, 3> arr1{'h', 'i', '\0'};
    my_array_t<char, 3> a = to_array("hi");

    ASSERT_EQ(arr1, a);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}