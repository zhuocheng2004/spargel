#pragma once

#include <spargel/base/initializer_list.h>

namespace spargel::base {

    template <typename T>
    class span {
    public:
        constexpr span() = default;

        explicit constexpr span(T const* begin, T const* end) : _begin{begin}, _end{end} {}
        constexpr span(initializer_list<T> l) : _begin{l.begin()}, _end{l.end()} {}

        constexpr T const& operator[](usize i) const { return _begin[i]; }

        constexpr usize count() const { return _end - _begin; }
        constexpr T const* data() const { return _begin; }

    private:
        T const* _begin = nullptr;
        T const* _end = nullptr;
    };

    template <typename T>
    span<T> make_span(usize count, T const* begin) {
        return span<T>(begin, begin + count);
    }

    template <typename T, usize N>
    span<T> make_span(T const (&arr)[N]) {
        return span<T>(arr, arr + N);
    }

}  // namespace spargel::base
