#ifndef MY_VECTOR_REALISATION__
#define MY_VECTOR_REALISATION__

#include <iostream>

template<typename T>
bool operator==(const my_vector_t<T>& rhs, const my_vector_t<T>& lhs) {
    if (lhs.size() != rhs.size()) {
        return false;
    }

    for (size_t i = 0; i < rhs.size(); ++i) {
        if (lhs[i] != rhs[i]) {
            return false;
        }
    }

    return true;
}


// TODO: building via tests cmake does not use global cmake, so
// this typedef won't execute
// #ifdef DEBUG_VERSION
#include <vector>

template<typename T>
void print_vector(const std::vector<T>& vec) {
    for (size_t i = 0; i < vec.size(); ++i) {
        std::cout << vec[i] << " ";
    }
    std::cout << std::endl;
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const my_vector_t<T>& vec) {
    for (size_t i = 0; i < vec.size(); ++i) {
        os << vec[i] << " ";
    }

    return os;
}

template<typename T>
bool operator==(const std::vector<T>& rhs, const my_vector_t<T>& lhs) {
    if (lhs.size() != rhs.size()) {
        return false;
    }

    for (size_t i = 0; i < rhs.size(); ++i) {
        if (lhs[i] != rhs[i]) {
            return false;
        }
    }

    return true;
}
// #endif // DEBUG_VERSION

#endif // MY_VECTOR_REALISATION__