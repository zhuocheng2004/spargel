#pragma once

#include <spargel/base/types.h>
#include <spargel/base/const.h>

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

#define SPARGEL_ENABLE_ASSERT 1

#if SPARGEL_ENABLE_ASSERT
#define ASSERT(cond) ((cond) ? (void)(0) : sbase_panic())
#else
#define ASSERT(cond)
#endif

/**
 * @brief a code path that shouldn't be reached
 */
// void sbase_unreachable() SPARGEL_ATTRIBUTE_NORETURN;

/**
 * @brief platform independent path to resources
 */
struct sbase_url {
    int _dummy;
};

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

/* backtrace */

void sbase_print_backtrace();

void sbase_panic() SPARGEL_ATTRIBUTE_NORETURN;

/* fiber (abandoned) */

/*
typedef void* sbase_fiber_context;

sbase_fiber_context sbase_create_fiber_context(void* stack, ssize stack_size,
                                               void (*func)());

void sbase_switch_fiber_context(sbase_fiber_context* from,
                                sbase_fiber_context to);
*/

/* task pool */

typedef struct sbase_task_pool* sbase_task_pool_id;
typedef u64 sbase_task_id;

struct sbase_task_descriptor {
    void (*callback)(void*);
    void* data;
};

sbase_task_pool_id sbase_create_task_pool();

sbase_task_id sbase_create_task(sbase_task_pool_id pool,
                                struct sbase_task_descriptor const* descriptor);
