#pragma once

#include <spargel/base/base.h>

/*
 * platform utilities
 */

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
ssize _spgl_get_executable_path(char* buf, ssize buf_size);

struct sbase_string spgl_get_executable_path();
