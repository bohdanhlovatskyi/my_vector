#ifndef MY_ARRAY__
#define MY_ARRAY__

#include <compare>
#include <iterator>

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
    inline T* data() const { return data_; }

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

    T operator[](size_t idx) const {
        return data_[idx];
    }

    T at(size_t idx) const {
        if (idx >= N) {
            throw std::range_error{"Index out of bounds"};
        }

        return data_[idx];
    }

    void swap(my_array_t& other) noexcept {
        std::swap(data_, other.data_);
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

#endif // MY_ARRAY__
