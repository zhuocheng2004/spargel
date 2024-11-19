#pragma once

#include <stddef.h>

namespace spargel::base {

/**
 * get the absolute path string of the executable file associated to the
 * currently running process
 *
 * When the actually path string is longer than buf_size, the content of the
 * buffer is undefined.
 *
 * @param buf the buffer to which the path string will be written to
 * @param buf_size the size of the buffer
 * @return the length of the path string; zero if the
 * path cannot be got
 */
size_t getExecutablePath(char* buf, size_t buf_size);

}  // namespace spargel::base
