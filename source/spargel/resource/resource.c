#include <spargel/resource/resource.h>

void spgl_resource_put(struct spgl_resource* resource)
{
    resource->ref_cnt--;
    if (resource->ref_cnt <= 0) resource->op->close(resource);
}
