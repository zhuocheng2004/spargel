#include <spargel/base/platform.h>

/* libc */
#include <stdio.h>

int main() {
    auto path = spargel::base::get_executable_path();
    printf("Current executable path: \"%s\" \n", path.data());
    printf("Path length: %ld\n", path.length());

    spargel::base::report_allocation();
    spargel::base::check_leak();
    return 0;
}
