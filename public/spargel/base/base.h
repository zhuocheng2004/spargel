#pragma once

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
#endif

/* signed size */
typedef ptrdiff_t ssize;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

/**
 * @brief a code path that shouldn't be reached
 */
void spargel_unreachable() SPARGEL_ATTRIBUTE_NORETURN;

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
typedef struct spargel_url {
} spargel_url;
