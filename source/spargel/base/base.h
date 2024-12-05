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

#if SPARGEL_ENABLE_CHECK
#define CHECK(cond) ((cond) ? (void)(0) : sbase_panic_here())
#else
#define CHECK(cond)
#endif

#if SPARGEL_ENABLE_DCHECK
#define DCHECK(cond) ((cond) ? (void)(0) : sbase_panic_here())
#else
#define DCHECK(cond)
#endif

/* panic */

void sbase_panic() SPARGEL_ATTRIBUTE_NORETURN;

void sbase_panic_at(char const* file, char const* func, ssize line) SPARGEL_ATTRIBUTE_NORETURN;

#define sbase_panic_here() sbase_panic_at(__FILE__, __func__, __LINE__)

/* logging */

enum sbase_log_level {
    /* message for debugging */
    SBASE_LOG_DEBUG = 0,
    /* general log events */
    SBASE_LOG_INFO,
    /* warning, not necessarily shown to users */
    SBASE_LOG_WARN,
    /* error that can recover from */
    SBASE_LOG_ERROR,
    /* nothing more can be done other than aborting */
    SBASE_LOG_FATAL,
};

void sbase_log(int level, char const* file, char const* func, ssize line, char const* format, ...)
    SPARGEL_ATTRIBUTE_PRINTF_FORMAT(5, 6);

#define SBASE_LOG_IMPL(level, ...) sbase_log(level, __FILE__, __func__, __LINE__, __VA_ARGS__)
#define sbase_log_debug(...) SBASE_LOG_IMPL(SBASE_LOG_DEBUG, __VA_ARGS__)
#define sbase_log_info(...) SBASE_LOG_IMPL(SBASE_LOG_INFO, __VA_ARGS__)
#define sbase_log_warn(...) SBASE_LOG_IMPL(SBASE_LOG_WARN, __VA_ARGS__)
#define sbase_log_error(...) SBASE_LOG_IMPL(SBASE_LOG_ERROR, __VA_ARGS__)
#define sbase_log_fatal(...)                          \
    do {                                              \
        SBASE_LOG_IMPL(SBASE_LOG_FATAL, __VA_ARGS__); \
        sbase_panic_here();                           \
    } while (0)

/* backtrace */

void sbase_print_backtrace();

/* allocator */

enum sbase_allocation_tag {
    SBASE_ALLOCATION_BASE = 0,
    SBASE_ALLOCATION_CODEC,
    SBASE_ALLOCATION_ECS,
    SBASE_ALLOCATION_GPU,
    SBASE_ALLOCATION_UI,

    _SBASE_ALLOCATION_COUNT,
};

void* sbase_allocate(ssize size, int tag);
void* sbase_reallocate(void* ptr, ssize old_size, ssize new_size, int tag);
void sbase_deallocate(void* ptr, ssize size, int tag);

void sbase_report_allocation();
void sbase_check_leak();

/* string */

struct sbase_string {
    ssize length;
    char* data;
};

#define sbase_string_from_literal(str) ((struct sbase_string){sizeof(str) - 1, str})

struct sbase_string sbase_string_from_range(char const* begin, char const* end);

bool sbase_string_is_equal(struct sbase_string lhs, struct sbase_string rhs);

void sbase_string_deinit(struct sbase_string str);

void sbase_string_copy(struct sbase_string* dst, struct sbase_string src);

struct sbase_string sbase_string_concat(struct sbase_string str1, struct sbase_string str2);
