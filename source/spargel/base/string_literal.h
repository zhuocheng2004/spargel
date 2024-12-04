#pragma once

#include <spargel/base/types.h>

namespace spargel::base {

    class StringLiteral {
    public:
        constexpr StringLiteral() = default;

        template <ssize N>
        constexpr StringLiteral(char const (&str)[N]) : _begin{str}, _end{str + N - 1} {}

        constexpr ssize Length() const { return _end - _begin; }
        constexpr char const* Data() const { return _begin; }

        constexpr char const* Begin() const { return _begin; }
        constexpr char const* End() const { return _end; }

        constexpr char operator[](ssize i) const { return _begin[i]; }

    private:
        char const* _begin = nullptr;
        char const* _end = nullptr;
    };

}  // namespace spargel::base
