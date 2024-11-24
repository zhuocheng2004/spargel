#pragma once

#include <spargel/base/base.h>

/**
 * todo:
 *  - bmp
 *  - png
 *  - openexr
 *  - ptex
 */

/* this is a hack */
struct spargel_codec_color4 {
  u8 b;
  u8 g;
  u8 r;
  u8 a;
};

struct spargel_codec_image {
  int width;
  int height;
  struct spargel_codec_color4* pixels;
};

enum spargel_codec_decode_result {
  SPARGEL_CODEC_DECODE_SUCCESS,
  /* todo: better error codes */
  SPARGEL_CODEC_DECODE_FAILED,
};

/**
 * @brief ppm loader
 */
int spargel_codec_load_ppm_image(
    char const* path, struct spargel_codec_image* image);

void spargel_codec_destroy_image(struct spargel_codec_image* image);
