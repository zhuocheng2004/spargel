#include <spargel/resource/empty.h>

using namespace spargel::resource;

int main() {
    struct resource_manager manager;
    resource_empty_manager_init(&manager);

    struct resource_id id = RESOURCE_ID("path/to/resource");
    spargel_assert(resource_open_resource(&manager, id) == nullptr);

    resource_close_manager(&manager);

    sbase_report_allocation();
    sbase_check_leak();
    return 0;
}
