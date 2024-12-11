#include <spargel/base/base.h>

int main() {
    spargel_defer(([] {
        spargel::base::report_allocation();
        spargel::base::check_leak();
    }));

    spargel_log_debug("hello");
    spargel_log_info("info with %d", 1);
    spargel_log_warn("hello with %s", "warn");
    spargel_log_error("hello");
    spargel_log_fatal("a fatal message");

    return 0;
}
