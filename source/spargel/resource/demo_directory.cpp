#include <spargel/resource/directory.h>

using namespace spargel::resource;

int main() {
    struct resource_manager manager;
    struct sbase_string base_path = sbase_string_from_literal(".");
    sbase_log_debug("base_path.length = %ld", base_path.length);
    resource_directory_manager_init(&manager, base_path);

    struct resource_id id = RESOURCE_ID("path/to/resource");
    spargel_assert(resource_open_resource(&manager, id) == nullptr);

    resource_close_manager(&manager);

    sbase_report_allocation();
    sbase_check_leak();
    return 0;
}
