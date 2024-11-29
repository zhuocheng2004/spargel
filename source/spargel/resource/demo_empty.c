
#include <assert.h>
#include <spargel/resource/empty.h>

int main()
{
    struct sresource_manager manager;
    sresource_empty_manager_init(&manager);

    struct sresource_id id = SRESOURCE_ID("path/to/resource");
    assert(sresource_open_resource(&manager, id) == NULL);

    sresource_close_manager(&manager);

    return 0;
}
