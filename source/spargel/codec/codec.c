#include <spargel/base/base.h>
#include <spargel/codec/codec.h>
#include <stdio.h>
#include <stdlib.h>

static bool read_file(char const* path, char** data, ssize* size) {
  FILE* file = fopen(path, "rb");
  if (!file) return NULL;
  if (fseek(file, 0, SEEK_END) == -1) {
    fclose(file);
    return false;
  }
  ssize len = ftell(file);
  if (len <= 0) {
    fclose(file);
    return false;
  }
  rewind(file);

  char* ptr = malloc(len);
  if (!ptr) {
    fclose(file);
    return false;
  }
  if (fread(ptr, len, 1, file) != 1) {
    free(ptr);
    fclose(file);
    return false;
  }
  *data = ptr;
  *size = len;
  return true;
}

struct ppm_parser {
  char* cur;
  char* end;
  int width;
  int height;
  int max_color;
  struct spargel_codec_color4* pixels;
};

static void eat_whitespace(struct ppm_parser* ctx) {
  while (ctx->cur < ctx->end) {
    char ch = *ctx->cur;
    if (ch == ' ' || ch == '\n' || ch == '\r' || ch == '\t') {
      ctx->cur++;
    } else if (ch == '#') {
      while (ctx->cur < ctx->end && *ctx->cur != '\n') {
        ctx->cur++;
      }
    } else {
      break;
    }
  }
}

/* the magic is P3 */
static bool parse_ppm_magic(struct ppm_parser* ctx) {
  eat_whitespace(ctx);
  if (ctx->cur + 2 > ctx->end) return false;
  if (ctx->cur[0] == 'P' && ctx->cur[1] == '3') {
    ctx->cur += 2;
    return true;
  }
  return false;
}

static bool parse_uint(struct ppm_parser* ctx, int* value) {
  eat_whitespace(ctx);
  if (ctx->cur >= ctx->end) return false;
  char ch = *ctx->cur;
  if (ch < '0' || ch > '9') return false;
  int result = 0;
  while (ctx->cur < ctx->end) {
    char ch = *ctx->cur;
    if (ch >= '0' && ch <= '9') {
      result = result * 10 + (ch - '0');
      ctx->cur++;
    } else {
      break;
    }
  }
  *value = result;
  return true;
}

static bool parse_ppm_info(struct ppm_parser* ctx) {
  eat_whitespace(ctx);
  if (!parse_uint(ctx, &ctx->width)) return false;
  eat_whitespace(ctx);
  if (!parse_uint(ctx, &ctx->height)) return false;
  eat_whitespace(ctx);
  if (!parse_uint(ctx, &ctx->max_color)) return false;
  return true;
}

static bool parse_ppm_pixels(struct ppm_parser* ctx) {
  ssize count = ctx->width * ctx->height;
  for (ssize i = 0; i < count; i++) {
    int r, g, b;
    if (!parse_uint(ctx, &r) || !parse_uint(ctx, &g) || !parse_uint(ctx, &b)) {
      return false;
    }
    /* todo: hack */
    ctx->pixels[i] = (struct spargel_codec_color4){b, g, r, 255};
  }
  return true;
}

int spargel_codec_load_ppm_image(char const* path,
                                 struct spargel_codec_image* image) {
  int result;

  char* data = NULL;
  ssize size = 0;
  if (!read_file(path, &data, &size)) return SPARGEL_CODEC_DECODE_FAILED;

  struct ppm_parser parser;
  parser.cur = data;
  parser.end = data + size;

  if (!parse_ppm_magic(&parser)) {
    result = SPARGEL_CODEC_DECODE_FAILED;
    goto free_data;
  }
  if (!parse_ppm_info(&parser)) {
    result = SPARGEL_CODEC_DECODE_FAILED;
    goto free_data;
  }

  parser.pixels = malloc(sizeof(struct spargel_codec_color4) * parser.width *
                         parser.height);
  if (!parser.pixels) {
    result = SPARGEL_CODEC_DECODE_FAILED;
    goto free_data;
  }
  if (!parse_ppm_pixels(&parser)) {
    free(parser.pixels);
    result = SPARGEL_CODEC_DECODE_FAILED;
    goto free_data;
  }

  image->width = parser.width;
  image->height = parser.height;
  image->pixels = parser.pixels;

  result = SPARGEL_CODEC_DECODE_SUCCESS;

free_data:
  free(data);
  return result;
}

void spargel_codec_destroy_image(struct spargel_codec_image* image) {
  free(image->pixels);
}
