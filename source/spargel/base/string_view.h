#pragma once

#include <spargel/base/types.h>

namespace spargel::base {

    class string_view {
    public:
        constexpr string_view() = default;
        constexpr string_view(char const* begin, char const* end) : _begin{begin}, _end{end} {}
        template <ssize N>
        constexpr string_view(char const (&str)[N]) : string_view(str, str + N - 1) {}

        constexpr char operator[](ssize i) const { return _begin[i]; }
        constexpr ssize length() const { return _end - _begin; }
        constexpr char const* begin() const { return _begin; }
        constexpr char const* data() const { return _begin; }

    private:
        char const* _begin = nullptr;
        char const* _end = nullptr;
    };

}  // namespace spargel::base
