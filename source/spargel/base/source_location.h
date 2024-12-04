#pragma once

#include <spargel/base/types.h>

namespace spargel::base {

    class SourceLocation {
    public:
        constexpr SourceLocation() = default;

        constexpr SourceLocation(char const* file, char const* func, u32 line)
            : _file{file}, _func{func}, _line{line} {}

        static constexpr SourceLocation Current(char const* file = __builtin_FILE(),
                                                char const* func = __builtin_FUNCTION(),
                                                u32 line = __builtin_LINE()) {
            return SourceLocation(file, func, line);
        }

        constexpr char const* File() const { return _file; }
        constexpr char const* Function() const { return _func; }
        constexpr u32 Line() const { return _line; }

    private:
        char const* _file = "<unknown>";
        char const* _func = "<unknown>";
        u32 _line = 0;
    };

}  // namespace spargel::base
