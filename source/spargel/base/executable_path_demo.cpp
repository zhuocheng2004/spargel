#include <spargel/base/platform.h>

/* libc */
#include <stdio.h>

int main() {
    spargel::base::string path = spargel::base::get_executable_path();
    auto cleanup = spargel::base::defer{[=] { spargel::base::destroy(path); }};

    printf("Current executable path: \"%s\" \n", path.data);
    printf("Path length: %ld\n", path.length);

    spargel::base::report_allocation();
    // spargel::base::check_leak();
    return 0;
}
