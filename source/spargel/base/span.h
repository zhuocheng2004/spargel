#pragma once

namespace spargel::base {

    template <typename T>
    class span {
    public:
        constexpr span() = default;

        explicit constexpr span(T* begin, T* end) : _begin{begin}, _end{end} {}


        T& operator[](ssize i) { return _begin[i]; }
        T const& operator[](ssize i) const { return _end[i]; }

        ssize count() const { return _end - _begin; }
        T* data() { return _begin; }
        T const* data() const { return _begin; }

    private:
        T* _begin = nullptr;
        T* _end = nullptr;
    };

}
