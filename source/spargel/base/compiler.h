#pragma once

#if defined(__has_builtin)
#define spargel_has_builtin(x) __has_builtin(x)
#else
#define spargel_has_builtin(x) 0
#endif

#if defined(__clang__)
#define SPARGEL_IS_CLANG 1
#elif defined(__GNUC__)
#define SPARGEL_IS_GCC 1
#elif defined(_MSC_VER)
#define SPARGEL_IS_MSVC 1
#else
#error unknown compiler
#endif

#if defined(SPARGEL_IS_CLANG) || defined(SPARGEL_IS_GCC)
#define SPARGEL_ATTRIBUTE_PRINTF_FORMAT(format_arg, params_arg) \
    [[gnu::format(printf, format_arg, params_arg)]]
#else
#define SPARGEL_ATTRIBUTE_PRINTF_FORMAT(format_arg, params_arg)
#endif
