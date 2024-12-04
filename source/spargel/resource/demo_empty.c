#include <spargel/resource/empty.h>

/* libc */
#include <assert.h>
#include <stddef.h>

int main() {
    struct spgl_resource_manager manager;
    spgl_resource_empty_manager_init(&manager);

    struct spgl_resource_id id = SPGL_RESOURCE_ID("path/to/resource");
    assert(spgl_resource_open_resource(&manager, id) == NULL);

    spgl_resource_close_manager(&manager);

    return 0;
}
