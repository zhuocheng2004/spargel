#include <spargel/codec/codec.h>

/* libc */
#include <stdio.h>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("usage: ppm_info <file>\n");
        return 1;
    }
    char* file = argv[1];
    struct spargel_codec_image image;
    int result = spargel_codec_load_ppm_image(file, &image);
    if (result != SPARGEL_CODEC_DECODE_SUCCESS) {
        printf("error: cannot parse ppm file %s\n", file);
        return 1;
    }
    printf("width = %d, height = %d\n", image.width, image.height);
    return 0;
}
