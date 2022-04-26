// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#ifndef MY_VECTOR__
#define MY_VECTOR__

#include <compare>
#include <iterator>

template<typename T>
class my_vector_t {
private:
    // there are some funcs below that rely on this order, you
    // should not change it
    size_t size_;
    size_t capacity_;
    T* data_;

    // thanks Oleg Farenyuk for the idea
    template<typename Iter, class Arg>
    inline static void construct(Iter arr, const Arg& val) noexcept( noexcept(T{val}) ) {
        new(&*arr) T( val );
    }

    // this relies on the convention that user will not provide
    // a type with destructor that could throw an exception
    template<class Iter>
    inline static void construct(Iter arr) noexcept ( noexcept(T{}) ) {
        new(&*arr) T{};
    }

    template<class Iter>
    inline static void destruct(Iter arr) noexcept {
        arr->~T();
    }

    // noexcept( noexcept(~T()) ) fails for std::string
    // error: invalid argument type 'std::__1::basic_string<char>' to unary expression
    inline static void destroy(T* arr, size_t size) noexcept {
        for (size_t i = 0; i < size; ++i) {
            destruct(arr + i);
        }
    }

    inline static bool needs_realloc(size_t size, size_t capacity) {
        if (capacity > static_cast<int>(size * 1.5)) {
            return true;
        }

        return false;
    }

    // NOTE: reserve should be called before this one
    // Credit: Oleg Farenyuk
    template<typename Iter>
    void shift_forward(Iter pos, size_t dist) {
        size_t mv_elm_c = end() - pos;
        ptrdiff_t hs = dist - mv_elm_c;

        // ..........
        //       ||||||....
        if (hs > 0) {
            // fill hole with default initialized values
            for (size_t i = 0; i < static_cast<size_t>(hs); ++i) {
                construct(end());
                size_++;
            }

            for (size_t i = 0; i < mv_elm_c; ++i) {
                construct(end(), *(pos + i));
                size_++;
            }

            return;
        }

        // ..........
        //     ||||......
        for (size_t i = 0; i < dist; ++i) {
            construct(end(), *(end() - dist));
            size_++;
        }

        for (size_t i = -hs; i > 0; --i) {
            construct(pos + dist + i - 1, *(pos + i - 1));
        }
    }

    template<typename Iter>
    void remove_block(Iter pos, size_t dist) {
        // ..........
        //      |||
        size_t mv = end() - pos;
        for (size_t i = 0; i < mv; ++i) {
            if (pos + dist + i < end()) {
                construct(pos + i, *(pos + dist + i));
            } else {
                construct(pos + i);
            }
        }

        for (size_t i = 0; i < dist; ++i) {
            destruct(end() - 1);
            size_--;
        }
    }

public:

    inline size_t size() const { return size_; }
    inline size_t capacity() const { return capacity_; }
    inline const T* data() const { return data_; }
    inline T* data() { return data_; }
    inline bool empty() const { return size_ == 0; }

    // default constructor
    my_vector_t(): data_{nullptr}, size_{0}, capacity_{0} {};
    my_vector_t(size_t size): my_vector_t(size, T{}) {}

    // constructor from n copies of given elm
    // note: apparently it would indeed be better to move this into
    // some memory management class rather than rely on the order
    // of the member initialization :+)
    my_vector_t(size_t size, const T& elm): size_{size}, capacity_{2 * size},
            data_{static_cast<T*>( ::operator new(capacity_ * sizeof(T)) )}
    {
        for (size_t i = 0; i < size; ++i) {
            construct(data_ + i, elm);
        }
    };

    // constructor of interval copy given by an iterator
    template<typename ForwardIter,
             typename = typename std::iterator_traits<ForwardIter>::value_type>
    my_vector_t(ForwardIter beg, ForwardIter end): \
                        size_{0},
                        capacity_{2 * (static_cast<size_t>(end - beg))},
                        data_{static_cast<T*>( ::operator new(capacity_ * sizeof(T)) )}
    {
        for (auto it = beg; it != end; ++it) {
            // *it would be rvalue anyway
            push_back(*it);
        }
    }

    // constructor with initialization list
    my_vector_t(std::initializer_list<T> ll): \
                size_{0}, capacity_{2 * ll.size()},
                data_{static_cast<T*>( ::operator new(capacity_ * sizeof(T)) )}
    {
        for (const auto& elm : ll) {
            push_back(elm);
        }
    }

    // copy constructor
    my_vector_t(const my_vector_t& vec): \
                size_{vec.size_}, capacity_{vec.size_},
                data_{static_cast<T*>( ::operator new(capacity_ * sizeof(T)) )} {
        for (size_t i = 0; i < vec.size_; ++i) {
            construct(data_ + i, vec[i]);
        }
    }

    // move constructor
    my_vector_t(my_vector_t&& vec) noexcept(noexcept(~T())) : \
                size_{0}, capacity_{0}, data_{nullptr}  {
        *this = std::move(vec);
    }

    // destructor
    ~my_vector_t() noexcept {
        destroy(data_, size_);
        ::operator delete(data_);
    }

    // assignment and move operator
    my_vector_t& operator=(my_vector_t&& vec) noexcept(noexcept(~T())) {
        if (this == &vec) {
            return *this;
        }

        destroy(data_, size_);
        ::operator delete(data_);

        data_ = vec.data_;
        size_ = vec.size_;
        capacity_ = vec.capacity_;

        vec.data_ = nullptr;
        vec.size_ = 0;
        vec.capacity_ = 0;

        return *this;
    }

    my_vector_t& operator=(const my_vector_t& vec) {
        if (this == &vec) {
            return *this;
        }

        destroy(data_, size_);
        size_ = vec.size_;
        capacity_ = vec.capacity_;
        data_ = static_cast<T*>( ::operator new(capacity_ * sizeof(T)) );
        std::copy(vec.data_, vec.data_ + vec.size_, data_);

        return *this;
    }

    void reserve(size_t new_capacity) {
        if (new_capacity <= capacity_) {
            return;
        }

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

    template<typename X>
    void push_back(X&& elm) {
        if (capacity_ == size_) {
            // capacity_ + 1 handles the case when capacity == 0
            reserve(2 * (capacity_ + 1));
            construct(data_ + size_, std::forward<X>(elm));
        } else {
            construct(data_ + size_, std::forward<X>(elm));
        }
        ++size_;
    }

    template<typename ...Args>
    void emplace_back(Args&&... args) {
        push_back(std::forward<T>(T{args...}));
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

    T& front() { return *data_; }

    T& back() { return *(data_ + size_ - 1); }

    T& operator[](size_t idx) {
        return data_[idx];
    }

    const T& operator[](size_t idx) const {
        return data_[idx];
    }

    T& at(size_t idx) {
        if (idx >= size_) {
            throw std::out_of_range{"at out of range"};
        }

        return data_[idx];
    }

    const T& at(size_t idx) const {
        if (idx >= size_) {
            throw std::out_of_range{"at out of range"};
        }

        return data_[idx];
    }

    void swap(my_vector_t<T>& other) noexcept {
        std::swap(data_, other.data_);
        std::swap(capacity_, other.capacity_);
        std::swap(size_, other.size_);
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

    // clear()
    // does not cause reallocation
    void clear() {
        destroy(data_, size_);
        size_ = 0;
    }

    // resize()
    void resize(size_t n, T val = T{}) {
        // if n is less than the current size
        while (size_ > n) {
            destruct(data_ + size_);
            --size_;
        }

        reserve(n);
        auto to_add = n - size_;
        for (size_t i = 0; i < to_add; ++i) {
            push_back(val);
        }
    }

    // stl compatible iterator
    template<typename C>
    class VecIter : public std::iterator<std::random_access_iterator_tag, C> {
    private:
        C* cur;
    public:
        using difference_type = typename std::iterator<std::random_access_iterator_tag, C>::difference_type;

        VecIter(): cur{nullptr} {}
        VecIter(C* data_ptr): cur{data_ptr} {}
        VecIter(const VecIter& rhs) { cur = rhs.cur; }

        VecIter& operator=(VecIter& other) { cur = other.cur; return *this; }

        C& operator*() { return *cur; }
        C* operator->() { return cur; }
        C& operator[](difference_type i) { return cur[i]; }

        VecIter& operator+=(difference_type i) { cur += i; return *this; }
        VecIter operator+(difference_type i) { return VecIter{cur + i}; }
        VecIter& operator-=(difference_type i) { cur -= i; return *this; }
        VecIter operator-(difference_type i) { return VecIter{cur - i}; }

        difference_type operator-(const VecIter& right) const noexcept{ return cur - right.cur; }

        VecIter& operator++() { cur++; return *this; }
        VecIter& operator++(int) {
            C tmp{*this};
            cur++;
            return tmp;
        }

        VecIter& operator--() { cur--; return *this; }
        VecIter& operator--(int) {
            C tmp{*this};
            cur--;
            return tmp;
        }

        friend bool operator==(const VecIter& it1, const VecIter& it2) {
            return it1.cur == it2.cur;
        }

        friend auto operator<=>(const VecIter& it1, const VecIter& it2) {
            return it1.cur <=> it2.cur;
        }
    };

    using iterator = VecIter<T>;
    iterator begin() { return iterator{data_}; }
    iterator end() { return iterator{data_ + size_}; }

    using reverse_iterator = std::reverse_iterator<VecIter<T>>;
    reverse_iterator rbegin() { return std::reverse_iterator(end()); }
    reverse_iterator rend() { return std::reverse_iterator(begin()); }

    using const_iterator = VecIter<const T>;
    const_iterator cbegin() { return const_iterator{data_}; }
    const_iterator cend() { return const_iterator{data_ + size_}; }

    using reverse_const_iterator = std::reverse_iterator<VecIter<const T>>;
    reverse_const_iterator rcbegin() {  return std::reverse_iterator(cend()); }
    reverse_const_iterator rcend() { return std::reverse_iterator(cbegin()); }

    template<typename Iter, class V>
    void insert(Iter pos, V&& val) {
        if (size_ + 1 > capacity_) {
            reserve(2 * capacity_);
        }

        shift_forward(pos, 1);
        construct(pos, std::forward<V>(val));
    }

    template<typename Iter>
    void insert(Iter pos, Iter from, Iter to) {
        size_t dist = static_cast<size_t>(to - from);
        if (size_ + dist > capacity_) {
            reserve(std::max(size_ + dist, 2 * capacity_));
        }

        shift_forward(pos, dist);
        while (from != to) {
            construct(pos, *from);
            ++pos; ++from;
        }
    }

    // analogous erase
    template<typename Iter>
    void erase(Iter pos) {
        remove_block(pos, 1);
    }

    template<typename Iter>
    void erase(Iter from, Iter to) {
        remove_block(from, static_cast<size_t>(to - from));
    }

    // ==, !=, <, <=, >, >===, !=, <, <=, >, >=
    friend auto operator<=>(const my_vector_t& rhs, const my_vector_t<T>& lhs) {
        return rhs.data_ <=> lhs.data_;
    };


    friend bool operator==(const my_vector_t& rhs, const my_vector_t& lhs) {
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

};

#include "my_vector.cpp.h"

#endif // MY_VECTOR__
