#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

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

/* signed size */
typedef ptrdiff_t ssize;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

/**
 * @brief a code path that shouldn't be reached
 */
void spargel_unreachable() SPARGEL_ATTRIBUTE_NORETURN;
void sbase_unreachable() SPARGEL_ATTRIBUTE_NORETURN;

/**
 * @brief get the absolute path of the executable file associated to the
 * currently process
 *
 * When the actually path string is longer than buf_size, the content of the
 * buffer is undefined.
 *
 * @param buf the buffer to which the path string will be written to
 * @param buf_size the size of the buffer
 * @return the length of the path string; zero if the
 * path cannot be got
 */
ssize spargel_get_executable_path(char* buf, ssize buf_size);

/**
 * @brief platform independent path to resources
 */
struct sbase_url {
  int _dummy;
};

struct sbase_string {
  char* data;
  ssize length;
};

#define sbase_string_from_literal(str) \
  ((struct sbase_string){sizeof(str) - 1, str})

struct sbase_string sbase_string_from_range(char const* begin, char const* end);

bool sbase_string_is_equal(struct sbase_string lhs, struct sbase_string rhs);

void sbase_string_deinit(struct sbase_string str);

/* backtrace */

void sbase_print_backtrace();

/* task pool */

typedef struct sbase_task_pool* sbase_task_pool_id;

sbase_task_pool_id sbase_create_task_pool();
