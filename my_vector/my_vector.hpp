#ifndef MY_VECTOR__
#define MY_VECTOR__

#include <compare>
#include <iterator>
#include <algorithm>
#include <assert.h>

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

    inline static bool needs_realloc(size_t size, size_t capacity) {
        if (capacity > static_cast<int>(size * 1.5)) {
            return true;
        }

        return false;
    }

    void shift_forward(T* pos, size_t dist) {
        ;
    }

    void shift_backward(T* pos, size_t dist) {
        ;
    }

public:

    inline size_t size() const { return size_; }
    inline size_t capacity() const { return capacity_; }
    inline T* data() const { return data_; }
    inline bool empty() const { return size_ == 0; }

    // default constructor
    my_vector_t(): data_{nullptr}, size_{0}, capacity_{0} {};

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
        static_assert(std::is_constructible_v<decltype(*beg)>());

        size_ = end - beg;
        capacity_ = static_cast<size_t>(2 * size_);
        assert(capacity_ >= 0);
        data_ = static_cast<T*>( ::operator new(capacity_ * sizeof(T)) );
        for (auto& it = beg; it != end; ++it) {
            construct(data_++, *it);
        }
    }

    // constructor with initialization list
    my_vector_t(std::initializer_list<T> l): my_vector_t{l.begin(), l.end()} {}

    // copy constructor
    my_vector_t(const my_vector_t& vec): my_vector_t(vec.begin(), vec.end()) {}

    // move constructor
    template<class V>
    my_vector_t(my_vector_t<V>&& vec): data_{std::forward<V>(vec.data_)},
                                       size_{std::move(size)},
                                       capacity_{std::move(capacity_)} {}

    // destructor
    ~my_vector_t() noexcept {
        for (size_t i = 0; i < size_; ++i) {
            destruct(data_ + i);
        }
        ::operator delete(data_);
    }

    // assignment operator
    my_vector_t& operator=(const my_vector_t& rhs) {
        ;
    }

    // move operator
    my_vector_t& operator=(my_vector_t&& rhs) {
        ;
    }

    void reserve(size_t new_capacity) {
        if (new_capacity <= capacity_) {
            return;
        }

        // TODO: this should be put in some other func
        my_vector_t<T> temp(0, T{});
        temp.data_ = static_cast<T*>( ::operator new(new_capacity * sizeof(T)) );
        temp.capacity_ = new_capacity;
        for (size_t i = 0; i < size_; ++i) {
            // node that this handles the case when element lays within
            // the array, as the data would be still valid
            construct(temp.data_ + i, T{data_[i]});
        }
        temp.size_ = size_;

        swap(temp);
    }

    // clear()

    // resize()

    template<typename V>
    void push_back(V&& elm) {

        if (capacity_ == size_) {
            // capacity_ + 1 handles the case when capacity == 0
            reserve(2 * (capacity_ + 1));
            construct(data_ + size_, std::forward<V>(elm));
        } else {
            construct(data_ + size_, std::forward<V>(elm));
        }
        ++size_;
    }

    // this won't free any memory, use shrink to fit, it necessary
    void pop_back() {
        // this little one changes from stdlib, though not sure
        // whether this affects performance that much
        if (size_ == 0) {
            return;
        }

        destruct(data_ + size_);
        --size_;
    }

    // may cause reallocation. It is guaranteed that elements won't be changed
    void shrink_to_fit() {
        if (this->needs_realloc(size_, capacity_)) {
            my_vector_t<T> vec(*this);
            swap(vec);
        } else {
            // we won't reallocate
            capacity_ = size_;
        }
    }

    T& front() noexcept { return data_; }
    T& back() noexcept { return data_ + size_; }

    T operator[](size_t idx) const {
        return data_[idx];
    }

    T at(size_t idx) const {
        if (idx >= size_) {
            throw std::range_error{"Index out of bounds"};
        }

        return data_[idx];
    }

    void swap(my_vector_t<T>& other) noexcept {
        std::swap(data_, other.data_);
        std::swap(capacity_, other.capacity_);
        std::swap(size_, other.size_);
    }

    // stl compatible iterator
    class VecIter : public std::iterator<std::random_access_iterator_tag, T> {
    private:
        T* cur;
    public:
        using difference_type = typename std::iterator<std::random_access_iterator_tag, T>::difference_type;

        VecIter(): cur{nullptr} {}
        VecIter(T* data_ptr): cur{data_ptr} {}
        VecIter(const VecIter& rhs) { cur = rhs.cur; }

        VecIter& operator=(VecIter& other) { cur = other.cur; return *this; }

        T& operator*() { return *cur; }
        T* operator->() { return cur; }
        T& operator[](difference_type i) { return cur[i]; }

        VecIter& operator+=(difference_type i) { cur += i; return *this; }
        VecIter operator+(difference_type i) { return VecIter{cur + i}; }
        VecIter& operator-=(difference_type i) { cur -= i; return *this; }
        VecIter operator-(difference_type i) { return VecIter{cur - i}; }

        difference_type operator-(const VecIter& right) const noexcept{ return cur - right.cur; }

        VecIter& operator++() { cur++; return *this; }
        VecIter& operator++(int) {
            T tmp{*this};
            cur++;
            return tmp;
        }

        VecIter& operator--() { cur--; return *this; }
        VecIter& operator--(int) {
            T tmp{*this};
            cur--;
            return tmp;
        }

        bool operator==(const VecIter& it) { return *cur == *(it.cur); }
        std::strong_ordering operator<=>(const VecIter& it) const noexcept { return *cur <=> *(it.cur); }

    };

    VecIter begin() noexcept(noexcept(VecIter())) { return VecIter{data_}; }
    VecIter end() noexcept(noexcept(VecIter())) { return VecIter{data_ + size_}; }

    using reverse_iterator = std::reverse_iterator<VecIter>;
    reverse_iterator rbegin() { return std::reverse_iterator<T>(end()); }
    reverse_iterator rend() { return std::reverse_iterator<T>(begin()); }

    // TODO: const iterators

    // accepts iterator and value
    template<typename V>
    VecIter insert(const VecIter& pos, V&& val) {
        ;
    }

    // accepts iterator and value
    // TODO: how to accept any iterator
    VecIter insert(const VecIter& pos, const VecIter& from, const VecIter& to) {
        ;
    }

    // remove one element
    // TODO: how should those behave if range is not valid
    void erase(const VecIter& pos) {
        ;
    }

    void erase(const VecIter& from, const VecIter& to) {
        ;
    }

    // TODO: test this
    bool operator==(const my_vector_t<T>& vec) {
        return std::equal(this->begin(), this->end(),
                          vec.begin(), vec.end());
    }

    // lexicographical_compare
    // TODO: test this
    bool operator<=>(const my_vector_t<T>& vec) {
        return std::lexicographical_compare(this->begin(), this->end(),
                                            vec.begin(), vec.end());
    }
};

#include "my_vector.cpp.h"

#endif // MY_VECTOR__
