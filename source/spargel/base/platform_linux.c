#include <unistd.h>
#include <stdio.h>

#include <spargel/base/base.h>

ssize _spgl_get_executable_path(char* buf, ssize buf_size)
{
    return readlink("/proc/self/exe", buf, buf_size);
}

void sbase_print_backtrace()
{
    printf("<unknown backtrace>\n");
}
