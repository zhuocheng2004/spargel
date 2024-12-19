#include <spargel/base/base.h>
#include <spargel/base/win_procs.h>

/* libc */
#include <stdio.h>

namespace spargel::base {

    usize _get_executable_path(char* buf, usize buf_size) {
        return GetModuleFileNameA(NULL, buf, buf_size);
    }

    void print_backtrace() { printf("<unknown backtrace>\n"); }

}  // namespace spargel::base
