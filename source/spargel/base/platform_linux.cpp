#include <spargel/base/base.h>

/* libc */
#include <stdio.h>
#include <unistd.h>

ssize _spgl_get_executable_path(char* buf, ssize buf_size) {
    return readlink("/proc/self/exe", buf, buf_size);
}

void sbase_print_backtrace() { printf("<unknown backtrace>\n"); }
