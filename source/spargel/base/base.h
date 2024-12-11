#pragma once

#include <spargel/base/const.h>
#include <spargel/base/types.h>
#include <spargel/config.h>

#if defined(__clang__)
#define SPARGEL_COMPILER_IS_CLANG
#elif defined(__GNUC__)
#define SPARGEL_COMPILER_IS_GCC
#elif defined(_MSC_VER)
#define SPARGEL_COMPILER_IS_MSVC
#else
#error unknown compiler
#endif

#if defined(SPARGEL_COMPILER_IS_CLANG) || defined(SPARGEL_COMPILER_IS_GCC)
#define SPARGEL_ATTRIBUTE_NORETURN __attribute__((noreturn))
#else
#define SPARGEL_ATTRIBUTE_NORETURN
#endif

#if defined(SPARGEL_COMPILER_IS_CLANG) || defined(SPARGEL_COMPILER_IS_GCC)
#define SPARGEL_ATTRIBUTE_PRINTF_FORMAT(format_arg, params_arg) \
    __attribute__((format(printf, format_arg, params_arg)))
#else
#define SPARGEL_ATTRIBUTE_PRINTF_FORMAT(format_arg, params_arg)
#endif

#if SPARGEL_ENABLE_ASSERT
#define spargel_assert(cond) ((cond) ? (void)(0) : spargel_panic_here())
#else
#define spargel_assert(cond)
#endif

namespace spargel::base {

    /* panic */

    [[noreturn]] void panic();

    [[noreturn]] void panic_at(char const* file, char const* func, ssize line);

#define spargel_panic_here() ::spargel::base::panic_at(__FILE__, __func__, __LINE__)

    /* logging */

    enum log_level {
        /* message for debugging */
        LOG_DEBUG = 0,
        /* general log events */
        LOG_INFO,
        /* warning, not necessarily shown to users */
        LOG_WARN,
        /* error that can recover from */
        LOG_ERROR,
        /* nothing more can be done other than aborting */
        LOG_FATAL,

        _LOG_COUNT,
    };

    void log(int level, char const* file, char const* func, ssize line, char const* format, ...)
        SPARGEL_ATTRIBUTE_PRINTF_FORMAT(5, 6);

#define LOG_IMPL(level, ...) ::spargel::base::log(level, __FILE__, __func__, __LINE__, __VA_ARGS__)
#define spargel_log_debug(...) LOG_IMPL(::spargel::base::LOG_DEBUG, __VA_ARGS__)
#define spargel_log_info(...) LOG_IMPL(::spargel::base::LOG_INFO, __VA_ARGS__)
#define spargel_log_warn(...) LOG_IMPL(::spargel::base::LOG_WARN, __VA_ARGS__)
#define spargel_log_error(...) LOG_IMPL(::spargel::base::LOG_ERROR, __VA_ARGS__)
#define spargel_log_fatal(...) LOG_IMPL(::spargel::base::LOG_FATAL, __VA_ARGS__);

    /* backtrace */

    void print_backtrace();

    /* allocator */

    enum allocation_tag {
        ALLOCATION_BASE = 0,
        ALLOCATION_CODEC,
        ALLOCATION_ECS,
        ALLOCATION_GPU,
        ALLOCATION_UI,

        _ALLOCATION_COUNT,
    };

    void* allocate(ssize size, int tag);
    void* reallocate(void* ptr, ssize old_size, ssize new_size, int tag);
    void deallocate(void* ptr, ssize size, int tag);

    void report_allocation();
    void check_leak();

    struct allocator {
        void* (*alloc)(ssize size, void* data);
        void* (*realloc)(void* old_ptr, ssize old_size, ssize new_size, void* data);
        void (*dealloc)(void* ptr, ssize size, void* data);
        void* data;
    };

    /* string */

    struct string {
        ssize length;
        char* data;
    };

#define string_from_literal(str) ((struct ::spargel::base::string){sizeof(str) - 1, str})

    string string_from_range(char const* begin, char const* end);

    bool operator==(string const& lhs, string const& rhs);

    void destroy(string const& str);

    void string_copy(string* dst, string src);

    string string_concat(string str1, string str2);

    template <typename F>
    struct defer {
        ~defer() { f(); }
        F f;
    };

}  // namespace spargel::base
