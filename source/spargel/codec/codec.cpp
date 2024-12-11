#include <spargel/base/base.h>
#include <spargel/codec/codec.h>

void scodec_destroy_image(struct spargel_codec_image image) {
    if (image.pixels) {
        spargel_assert(image.width > 0 && image.height > 0);
        sbase_deallocate(image.pixels,
                         sizeof(struct spargel_codec_color4) * image.width * image.height,
                         SBASE_ALLOCATION_CODEC);
    }
}
