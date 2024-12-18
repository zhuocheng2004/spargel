#include <spargel/base/base.h>

/* libc */
#include <stdio.h>
#include <unistd.h>

namespace spargel::base {

    usize _get_executable_path(char* buf, usize buf_size) {
        return readlink("/proc/self/exe", buf, buf_size);
    }

    void print_backtrace() { printf("<unknown backtrace>\n"); }

}  // namespace spargel::base
