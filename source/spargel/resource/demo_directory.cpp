#include <assert.h>

import spargel.base.c;

#include <spargel/resource/directory.h>

int main()
{
    struct spgl_resource_manager manager;
    struct sbase_string base_path = sbase_string_from_literal(".");
    spgl_resource_directory_manager_init(&manager, base_path);

    struct spgl_resource_id id = SPGL_RESOURCE_ID("path/to/resource");
    assert(spgl_resource_open_resource(&manager, id) == NULL);

    spgl_resource_close_manager(&manager);

    return 0;
}
