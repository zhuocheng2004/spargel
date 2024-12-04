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
int spargel_codec_load_ppm_image(char const* path, struct spargel_codec_image* image);

void spargel_codec_destroy_image(struct spargel_codec_image const* image);

/* json */

enum scodec_json_value_kind {
    SCODEC_JSON_VALUE_KIND_OBJECT,
    SCODEC_JSON_VALUE_KIND_ARRAY,
    SCODEC_JSON_VALUE_KIND_BOOLEAN,
    SCODEC_JSON_VALUE_KIND_STRING,
};

struct scodec_json_array {
    struct scodec_json_value* values;
    ssize count;
    ssize capacity;
};

struct scodec_json_object {
    struct scodec_json_object_entry* entries;
    ssize count;
    ssize capacity;
};

struct scodec_json_value {
    int kind;
    union {
        struct sbase_string string;
        bool boolean;
        struct scodec_json_array array;
        struct scodec_json_object object;
    };
};

struct scodec_json_object_entry {
    bool used;
    struct sbase_string key;
    u32 key_hash;
    struct scodec_json_value value;
};

void scodec_json_array_init(struct scodec_json_array* array);
struct scodec_json_value* scodec_json_array_push(struct scodec_json_array* array);
void scodec_json_array_deinit(struct scodec_json_array const* array);

void scodec_json_object_init(struct scodec_json_object* object);
struct scodec_json_value* scodec_json_object_insert(struct scodec_json_object* object,
                                                    struct sbase_string key);
/**
 * @warning the returned pointer is not stable
 */
struct scodec_json_value* scodec_json_object_get(struct scodec_json_object* object,
                                                 struct sbase_string key);
void scodec_json_object_deinit(struct scodec_json_object const* object);

void scodec_json_value_deinit(struct scodec_json_value const* value);

enum scodec_json_parse_result {
    SCODEC_JSON_PARSE_RESULT_SUCCESS,
    SCODEC_JSON_PARSE_RESULT_UNKNOWN_ERROR,
    SCODEC_JSON_PARSE_RESULT_UNSUPPORTED,
    SCODEC_JSON_PARSE_RESULT_EXPECT_RIGHT_SQUARE,
    SCODEC_JSON_PARSE_RESULT_EXPECT_RIGHT_CURLY,
    SCODEC_JSON_PARSE_RESULT_EXPECT_VALUE,
    SCODEC_JSON_PARSE_RESULT_EXPECT_END_OF_STRING,
    SCODEC_JSON_PARSE_RESULT_EXPECT_TRUE,
    SCODEC_JSON_PARSE_RESULT_EXPECT_FALSE,
    SCODEC_JSON_PARSE_RESULT_EXPECT_COLON,
};

int scodec_json_parse(char const* str, ssize len, struct scodec_json_value* value);
