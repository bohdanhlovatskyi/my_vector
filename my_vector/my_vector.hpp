#ifndef MY_VECTOR__
#define MY_VECTOR__

template<typename T>
class my_vector_t {
private:

    // TODO: consider using smart pointer
    T* data_;
    size_t capacity_;
    size_t size_;

    // thanks Oleg Farenyuk for the idea
    template<typename Arg>
    inline static void construct(T* arr, const Arg& val) noexcept( noexcept(T(val)) ) {
        new(arr) T( val );
    }

    inline static void construct(T* arr) noexcept {
        new(arr) T{};
    }

    inline static void destruct(T* arr) noexcept( noexcept(~T()) ) {
        arr->~T();
    }

    void swap(my_vector_t<T>& other) {
        std::swap(data_, other.data_);
        std::swap(capacity_, other.capacity_);
        std::swap(size_, other.size_);
    }

public:

    inline size_t size() const { return size_; }
    inline size_t get_capacity() const { return capacity_; }
    inline T* get_data() const { return data_; }

    // default constructor
    my_vector_t(): my_vector_t(0) {};

    // remember on the order of class vars initialization
    my_vector_t(size_t size): capacity_(2 * size), size_(size) {
        data_ = static_cast<T*>( ::operator new(capacity_ * sizeof(T)) );

        for (size_t i = 0; i < size_; ++i) {
            construct(data_ + i);
        }
    };

    // constructor from n copies of given elm
    // TODO: copy of default element won't be nice here ? (code reusability)
    my_vector_t(size_t size, T elm) {
        data_ = static_cast<T*>( ::operator new(capacity_ * sizeof(T)) );

        for (size_t i = 0; i < size; ++i) {
            construct(data_ + i, elm);
        }
    };

    // constructor of interval copy given by an iterator
    template<class ForwardIter>
    my_vector_t(ForwardIter beg, ForwardIter end) {
        ;
    }

    // constructor with initialization list

    // copy constructor
    // TODO : test this and maybe rewrite
    my_vector_t(const my_vector_t& vec) {
        if (vec.size_ > capacity_) {
            capacity_ = vec.capacity_;
            data_ = static_cast<T*>( ::operator new(capacity_ * sizeof(T)) );
        }

        for (size_t i = 0; i < vec.size_; ++i) {
            construct(data_ + i, vec[i]);
        }

        size_ = vec.size_;
    }

    // move constructor

    // destructor
    ~my_vector_t() noexcept {
        for (size_t i = 0; i < size_; ++i) {
            destruct(data_ + i);
        }
        ::operator delete(data_);
    }

    // assignment and move operator

    void reserve(size_t new_capacity) {
        if (new_capacity <= capacity_) {
            return;
        }

        // TODO: this should be put in some other func
        my_vector_t<T> temp(0, T{});
        temp.data_ = static_cast<T*>( ::operator new(new_capacity * sizeof(T)) );
        temp.capacity_ = new_capacity;

        for (size_t i = 0; i < size_; ++i) {
            construct(temp.data_ + i, T{data_[i]});
        }
        temp.size_ = size_;

        swap(temp);
    }

    void push_back(const T& elm) {

        if (capacity_ == size_) {
            // TODO: add check mentioned in the lecture
            reserve(2 * capacity_);
            construct(data_ + size_, elm);
        } else {
            construct(data_ + size_, elm);
        }
        ++size_;
    }


    void pop_back() {
        // consider memory freeing
        destruct(data_ + size_);
        --size_;
    }


    // front()

    // back()

    // operator[]


    // at (same as operator[], but throws an exception)
    T operator[](size_t idx) const {
        return data_[idx];
    }

    // stl compatible iterator
    // begin(), end(), cbegin(), cend(), rbegin(), rend(), rcbegin(), rcend()

    // s_empty(), (стандартна назва таки невдала), size(), reserve(), capacity(), shrink_to_fit()

    // swap()

    // clear()

    // resize()
};

#include "my_vector.cpp.h"

#endif // MY_VECTOR__
