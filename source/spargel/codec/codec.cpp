#include <spargel/base/base.h>
#include <spargel/codec/codec.h>

namespace spargel::codec {

    void destroy_image(struct image image) {
        if (image.pixels) {
            spargel_assert(image.width > 0 && image.height > 0);
            spargel::base::deallocate(image.pixels,
                                      sizeof(struct color4) * image.width * image.height,
                                      spargel::base::ALLOCATION_CODEC);
        }
    }

}  // namespace spargel::codec
