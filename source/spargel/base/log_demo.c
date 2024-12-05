#include <spargel/base/base.h>

int main() {
    sbase_log_debug("hello");
    sbase_log_info("info with %d", 1);
    sbase_log_warn("hello with %s", "warn");
    sbase_log_error("hello");
    sbase_log_fatal("a fatal message");

    sbase_report_allocation();
    sbase_check_leak();
    return 0;
}
