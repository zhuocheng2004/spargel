#include <spargel/resource/resource.h>

void sresource_put(struct sresource* resource)
{
    resource->ref_cnt--;
    if (resource->ref_cnt <= 0) resource->op->close(resource);
}
