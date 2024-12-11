#include <spargel/base/base.h>
#include <spargel/codec/codec.h>
#include <spargel/codec/cursor.h>

/* libc */
#include <stdio.h>
#include <stdlib.h>

namespace spargel::codec {

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

        char* ptr = (char*)spargel::base::allocate(len, spargel::base::ALLOCATION_CODEC);
        if (!ptr) {
            fclose(file);
            return false;
        }
        if (fread(ptr, len, 1, file) != 1) {
            spargel::base::deallocate(ptr, len, spargel::base::ALLOCATION_CODEC);
            fclose(file);
            return false;
        }
        *data = ptr;
        *size = len;
        return true;
    }

    struct ppm_parser {
        struct cursor cursor;
        int width;
        int height;
        int max_color;
        struct color4* pixels;
    };

    static void eat_whitespace(struct ppm_parser* ctx) {
        struct cursor* cursor = &ctx->cursor;
        while (!cursor_is_end(cursor)) {
            char ch = cursor_peek(cursor);
            if (ch == ' ' || ch == '\n' || ch == '\r' || ch == '\t') {
                cursor_advance(cursor);
            } else if (ch == '#') {
                while (!cursor_is_end(cursor) && cursor_peek(cursor) != '\n') {
                    cursor_advance(cursor);
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
        return cursor_try_eat_bytes(&ctx->cursor, magic, 2);
    }

    static bool parse_uint(struct ppm_parser* ctx, int* value) {
        eat_whitespace(ctx);
        struct cursor* cursor = &ctx->cursor;
        if (cursor_is_end(cursor)) return false;
        char ch = cursor_peek(cursor);
        if (ch < '0' || ch > '9') return false;
        int result = 0;
        while (!cursor_is_end(cursor)) {
            char ch = cursor_peek(cursor);
            if (ch >= '0' && ch <= '9') {
                result = result * 10 + (ch - '0');
                cursor_advance(cursor);
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
            ctx->pixels[i] = (struct color4){(u8)b, (u8)g, (u8)r, 255};
        }
        return true;
    }

    int load_ppm_image(char const* path, struct image* image) {
        int result;

        char* data = NULL;
        ssize size = 0;
        if (!read_file(path, &data, &size)) return DECODE_FAILED;

        struct ppm_parser parser;
        parser.cursor.cur = data;
        parser.cursor.end = data + size;

        if (!parse_ppm_magic(&parser)) {
            result = DECODE_FAILED;
            goto free_data;
        }
        if (!parse_ppm_info(&parser)) {
            result = DECODE_FAILED;
            goto free_data;
        }

        parser.pixels = (struct color4*)spargel::base::allocate(
            sizeof(struct color4) * parser.width * parser.height, spargel::base::ALLOCATION_CODEC);
        if (!parser.pixels) {
            result = DECODE_FAILED;
            goto free_data;
        }
        if (!parse_ppm_pixels(&parser)) {
            free(parser.pixels);
            result = DECODE_FAILED;
            goto free_data;
        }

        image->width = parser.width;
        image->height = parser.height;
        image->pixels = parser.pixels;

        result = DECODE_SUCCESS;

    free_data:
        spargel::base::deallocate(data, size, spargel::base::ALLOCATION_CODEC);
        return result;
    }

    void destroy_image(struct image const* image) { free(image->pixels); }

}  // namespace spargel::codec
