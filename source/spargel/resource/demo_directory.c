#include <spargel/resource/directory.h>

/* libc */
#include <assert.h>
#include <stddef.h>

int main() {
    struct spgl_resource_manager manager;
    struct sbase_string base_path = sbase_string_from_literal(".");
    sbase_log_debug("base_path.length = %ld", base_path.length);
    spgl_resource_directory_manager_init(&manager, base_path);

    struct spgl_resource_id id = SPGL_RESOURCE_ID("path/to/resource");
    assert(spgl_resource_open_resource(&manager, id) == NULL);

    spgl_resource_close_manager(&manager);

    sbase_report_allocation();
    sbase_check_leak();
    return 0;
}
