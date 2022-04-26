#ifndef MY_ARRAY__
#define MY_ARRAY__

#include <compare>
#include <iterator>
#include <type_traits>

template<typename T, size_t N>
class my_array_t {
private:
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

public:

    inline size_t size() const { return N; }
    inline const T& data() const noexcept { return data_; }

    // may or may not return nullptr if N == 0
    inline T& data() noexcept { return data_; }

    // default initializer, leaves the elements of data uninitialized
    // to request default value initialization, call my_array_t a{};
    my_array_t(): data_{static_cast<T*>( ::operator new(N * sizeof(T)) )} {}

    // constructor with initialization list
    my_array_t(std::initializer_list<T> ll): \
                data_{static_cast<T*>( ::operator new(N * sizeof(T)) )}
    {
        size_t i = 0;
        for (const auto& elm : ll) {
            construct(data_ + i, elm);
            i++;

            // TODO: do we need this one ? 
            if (i > N) { break; }
        }

        while (i < N) {
            construct(data_ + i);
            i++;
        }
    }

    ~my_array_t() noexcept {
        destroy(data_, N);
        ::operator delete(data_);
    }

    my_array_t(my_array_t&& vec) = default;
    my_array_t(const my_array_t& vec) = default;
    my_array_t& operator=(my_array_t&& vec) = default;
    my_array_t& operator=(const my_array_t& vec) = default;

    // begin() == end() 
    inline bool empty() const noexcept { return N == 0; }

    T& front() { return *data_; }

    T& back() { return *(data_ + N - 1); }

    T& operator[](size_t idx) {
        return data_[idx];
    }

    const T& operator[](size_t idx) const {
        return data_[idx];
    }

    T& at(size_t idx) {
        if (idx >= N) {
            throw std::out_of_range{"at out of range"};
        }

        return data_[idx];
    }

    const T& at(size_t idx) const {
        if (idx >= N) {
            throw std::out_of_range{"at out of range"};
        }

        return data_[idx];
    }

    constexpr void fill(const T& elm) {
        destroy(data_, N);
        for (size_t i = 0; i < N; ++i) {
            construct(data_ + i, elm);
        }
    }

    // O_o swap for array is linear, who could imagine that ? 
    void swap(my_array_t& other) noexcept (std::is_nothrow_swappable_v<T>) {
        my_array_t<T, N> tmp;
        for (size_t i = 0; i < other.size(); ++i) {
            tmp[i] = data_[i];
        }

        for (size_t i = 0; i < other.size(); ++i) {
            data_[i] = other.data_[i];
        }

        for (size_t i = 0; i < other.size(); ++i) {
            other.data_[i] = tmp.data_[i];
        }
    }

    // stl compatible iterator
    template<typename C>
    class ArrIter : public std::iterator<std::random_access_iterator_tag, C> {
    private:
        C* cur;
    public:
        using difference_type = typename std::iterator<std::random_access_iterator_tag, C>::difference_type;

        ArrIter(): cur{nullptr} {}
        ArrIter(C* data_ptr): cur{data_ptr} {}
        ArrIter(const ArrIter& rhs) { cur = rhs.cur; }

        ArrIter& operator=(ArrIter& other) { cur = other.cur; return *this; }

        C& operator*() { return *cur; }
        C* operator->() { return cur; }
        C& operator[](difference_type i) { return cur[i]; }

        ArrIter& operator+=(difference_type i) { cur += i; return *this; }
        ArrIter operator+(difference_type i) { return ArrIter{cur + i}; }
        ArrIter& operator-=(difference_type i) { cur -= i; return *this; }
        ArrIter operator-(difference_type i) { return ArrIter{cur - i}; }

        difference_type operator-(const ArrIter& right) const noexcept{ return cur - right.cur; }

        ArrIter& operator++() { cur++; return *this; }
        ArrIter& operator++(int) {
            C tmp{*this};
            cur++;
            return tmp;
        }

        ArrIter& operator--() { cur--; return *this; }
        ArrIter& operator--(int) {
            C tmp{*this};
            cur--;
            return tmp;
        }

        friend bool operator==(const ArrIter& it1, const ArrIter& it2) {
            return it1.cur == it2.cur;
        }

        friend auto operator<=>(const ArrIter& it1, const ArrIter& it2) {
            return it1.cur <=> it2.cur;
        }
    };

    using iterator = ArrIter<T>;
    iterator begin() { return iterator{data_}; }
    iterator end() { return iterator{data_ + N}; }

    using reverse_iterator = std::reverse_iterator<ArrIter<T>>;
    reverse_iterator rbegin() { return std::reverse_iterator(end()); }
    reverse_iterator rend() { return std::reverse_iterator(begin()); }

    using const_iterator = ArrIter<const T>;
    const_iterator cbegin() { return const_iterator{data_}; }
    const_iterator cend() { return const_iterator{data_ + N}; }

    using reverse_const_iterator = std::reverse_iterator<ArrIter<const T>>;
    reverse_const_iterator rcbegin() {  return std::reverse_iterator(cend()); }
    reverse_const_iterator rcend() { return std::reverse_iterator(cbegin()); }

    // ==, !=, <, <=, >, >===, !=, <, <=, >, >=
    friend auto operator<=>(const my_array_t& rhs, const my_array_t& lhs) {
        return rhs.data_ <=> lhs.data_;
    };

    friend bool operator==(const my_array_t& rhs, const my_array_t& lhs) {
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

template<class T, std::size_t N>
constexpr my_array_t<std::remove_cv_t<T>, N> to_array(T (&a)[N]) { 
    my_array_t<std::remove_cv_t<T>, N> arr;
    for (size_t i = 0; i < N; ++i) {
        arr[i] = a[i];
    }

    return arr;
}

#endif // MY_ARRAY__
