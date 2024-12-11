#include <spargel/base/platform.h>

/* libc */
#include <stdio.h>

int main() {
    spargel_defer(([] {
        spargel::base::report_allocation();
        spargel::base::check_leak();
    }));

    auto path = spargel::base::get_executable_path();
    spargel_defer(([&] { path.deinit(); }));

    printf("Current executable path: \"%s\" \n", path.data);
    printf("Path length: %ld\n", path.length);

    return 0;
}
