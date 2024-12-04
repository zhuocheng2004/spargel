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

/* backtrace */

void sbase_print_backtrace();

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
