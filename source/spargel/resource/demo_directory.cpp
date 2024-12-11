#include <spargel/resource/directory.h>

using namespace spargel::resource;

int main() {
    struct resource_manager manager;
    spargel::base::string base_path = string_from_literal(".");
    spargel_log_debug("base_path.length = %ld", base_path.length);
    resource_directory_manager_init(&manager, base_path);

    struct resource_id id = RESOURCE_ID("path/to/resource");
    spargel_assert(resource_open_resource(&manager, id) == nullptr);

    resource_close_manager(&manager);

    spargel::base::report_allocation();
    spargel::base::check_leak();
    return 0;
}
