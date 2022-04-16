// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <iostream>
#include <vector>

#include "my_vector/my_vector.hpp"

int main(int argc, char* argv[]) {
    (void) argc; (void) argv;

    my_vector_t<int> vec;
    std::cout << vec.size() << std::endl;
    for (size_t i = 0; i < 5; ++i) {
        vec.push_back(i);
    }

    for (size_t i = 0; i < vec.size(); ++i) {
        std::cout << vec[i] << ", ";
    }
    std::cout << std::endl;

    std::vector<int> vec2(10);
    std::cout << vec2.size() << std::endl;
    for (size_t i = 0; i < 5; ++i) {
        vec2.push_back(i);
    }

    vec2[2] = 9;

    for (size_t i = 0; i < vec2.size(); ++i) {
        std::cout << vec2[i] << ", ";
    }
    std::cout << std::endl;

    return 0;
}
