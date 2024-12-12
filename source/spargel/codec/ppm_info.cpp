#include <spargel/base/logging.h>
#include <spargel/codec/codec.h>

/* libc */
#include <stdio.h>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("usage: ppm_info <file>\n");
        return 1;
    }
    char* file = argv[1];
    struct spargel::codec::image image;
    int result = spargel::codec::load_ppm_image(file, &image);
    if (result != spargel::codec::DECODE_SUCCESS) {
        spargel_log_error("cannot parse ppm file %s\n", file);
        return 1;
    }
    printf("width = %d, height = %d\n", image.width, image.height);

    spargel::codec::destroy_image(image);

    spargel::base::check_leak();
    spargel::base::check_leak();
    return 0;
}
