#include <spargel/base/platform.h>

/* libc */
#include <stdio.h>

int main() {
    struct sbase_string path = spgl_get_executable_path();
    printf("Current executable path: \"%s\" \n", path.data);
    printf("Path length: %ld\n", path.length);
    sbase_string_deinit(path);

    sbase_report_allocation();
    sbase_check_leak();
    return 0;
}
