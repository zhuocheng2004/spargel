#pragma once

#include <spargel/base/compiler.h>

namespace spargel::base {

    struct source_location {
#if spargel_has_builtin(__builtin_FILE) && spargel_has_builtin(__builtin_FUNCTION) && \
    spargel_has_builtin(__builtin_LINE)
        static constexpr source_location current(char const* file = __builtin_FILE(),
                                                 char const* func = __builtin_FUNCTION(),
                                                 int line = __builtin_LINE()) {
            return {file, func, line};
        }
#else
#error unimplemented
#endif
        char const* _file;
        char const* _func;
        int _line;
    };

}  // namespace spargel::base