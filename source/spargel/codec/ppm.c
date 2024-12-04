#include <spargel/base/base.h>
#include <spargel/codec/codec.h>
#include <spargel/codec/cursor.h>

/* libc */
#include <stdio.h>
#include <stdlib.h>

static bool read_file(char const* path, char** data, ssize* size) {
    FILE* file = fopen(path, "rb");
    if (!file) return false;
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

    char* ptr = (char*)malloc(len);
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
    struct scodec_cursor cursor;
    int width;
    int height;
    int max_color;
    struct spargel_codec_color4* pixels;
};

static void eat_whitespace(struct ppm_parser* ctx) {
    struct scodec_cursor* cursor = &ctx->cursor;
    while (!scodec_cursor_is_end(cursor)) {
        char ch = scodec_cursor_peek(cursor);
        if (ch == ' ' || ch == '\n' || ch == '\r' || ch == '\t') {
            scodec_cursor_advance(cursor);
        } else if (ch == '#') {
            while (!scodec_cursor_is_end(cursor) && scodec_cursor_peek(cursor) != '\n') {
                scodec_cursor_advance(cursor);
            }
        } else {
            break;
        }
    }
}

/* the magic is P3 */
static bool parse_ppm_magic(struct ppm_parser* ctx) {
    eat_whitespace(ctx);
    static u8 const magic[] = "P3";
    return scodec_cursor_try_eat_bytes(&ctx->cursor, magic, 2);
}

static bool parse_uint(struct ppm_parser* ctx, int* value) {
    eat_whitespace(ctx);
    struct scodec_cursor* cursor = &ctx->cursor;
    if (scodec_cursor_is_end(cursor)) return false;
    char ch = scodec_cursor_peek(cursor);
    if (ch < '0' || ch > '9') return false;
    int result = 0;
    while (!scodec_cursor_is_end(cursor)) {
        char ch = scodec_cursor_peek(cursor);
        if (ch >= '0' && ch <= '9') {
            result = result * 10 + (ch - '0');
            scodec_cursor_advance(cursor);
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
        ctx->pixels[i] = (struct spargel_codec_color4){(u8)b, (u8)g, (u8)r, 255};
    }
    return true;
}

int spargel_codec_load_ppm_image(char const* path, struct spargel_codec_image* image) {
    int result;

    char* data = NULL;
    ssize size = 0;
    if (!read_file(path, &data, &size)) return SPARGEL_CODEC_DECODE_FAILED;

    struct ppm_parser parser;
    parser.cursor.cur = data;
    parser.cursor.end = data + size;

    if (!parse_ppm_magic(&parser)) {
        result = SPARGEL_CODEC_DECODE_FAILED;
        goto free_data;
    }
    if (!parse_ppm_info(&parser)) {
        result = SPARGEL_CODEC_DECODE_FAILED;
        goto free_data;
    }

    parser.pixels = malloc(sizeof(struct spargel_codec_color4) * parser.width * parser.height);
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

void spargel_codec_destroy_image(struct spargel_codec_image const* image) { free(image->pixels); }
