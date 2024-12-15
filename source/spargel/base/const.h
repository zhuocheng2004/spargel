#pragma once

#include <spargel/config.h>

/*
 * These values are only hints.
 * Do not use these values to create static buffers.
 * Use dynamic-size buffers.
 */
#undef PATH_MAX
#if SPARGEL_IS_ANDROID || SPARGEL_IS_LINUX
#define PATH_MAX 4096
#elif SPARGEL_IS_MACOS
#define PATH_MAX 1024
#elif SPARGEL_IS_WINDOWS
// obselete
#define PATH_MAX 260
#endif

#if SPARGEL_IS_WINDOWS
#define PATH_SPLIT '\\'
#else
#define PATH_SPLIT '/'
#endif
