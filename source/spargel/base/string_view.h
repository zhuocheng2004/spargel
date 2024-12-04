#pragma once

#include <spargel/base/string_literal.h>

namespace spargel::base {

    class PrintTarget;

    /// Read-only
    class StringView {
    public:
        constexpr StringView() = default;

        constexpr StringView(StringLiteral str) : _begin{str.Begin()}, _end{str.End()} {}

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
