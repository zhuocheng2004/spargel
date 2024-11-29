#pragma once

/*
 * These values are only hints.
 * Do not use these values to create static buffers.
 * Use dynamic-size buffers.
 */
#undef PATH_MAX
#if defined(SPARGEL_IS_LINUX)
#define PATH_MAX 4096
#elif defined(SPARGEL_IS_MACOS)
#define PATH_MAX 1024
#elif defined(SPARGEL_IS_WINDOWS)
// obselete
#define PATH_MAX 260
#endif

#if defined(SPARGEL_IS_WINDOWS)
#define PATH_SPLIT '\\'
#else
#define PATH_SPLIT '/'
#endif
