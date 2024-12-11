#include <spargel/resource/resource.h>

namespace spargel::resource {

    void resource_put(struct resource* resource) {
        resource->ref_cnt--;
        if (resource->ref_cnt <= 0) resource->op->close(resource);
    }

}  // namespace spargel::resource
