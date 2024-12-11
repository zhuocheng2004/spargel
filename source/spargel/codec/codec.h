#pragma once

#include <spargel/base/base.h>

namespace spargel::codec {

    /**
     * todo:
     *  - bmp
     *  - png
     *  - openexr
     *  - ptex
     */

    /* this is a hack */
    struct color4 {
        u8 b;
        u8 g;
        u8 r;
        u8 a;
    };

    struct image {
        int width;
        int height;
        struct color4* pixels;
    };

    void destroy_image(struct image image);

    enum decode_result {
        DECODE_SUCCESS,
        /* todo: better error codes */
        DECODE_FAILED,
    };

    /**
     * @brief ppm loader
     */
    int load_ppm_image(char const* path, struct image* image);

    void destroy_image(struct image const* image);

    /* json */

    enum json_value_kind {
        JSON_VALUE_KIND_OBJECT,
        JSON_VALUE_KIND_ARRAY,
        JSON_VALUE_KIND_BOOLEAN,
        JSON_VALUE_KIND_STRING,
    };

    struct json_array {
        struct json_value* values;
        ssize count;
        ssize capacity;
    };

    struct json_object {
        struct json_object_entry* entries;
        ssize count;
        ssize capacity;
    };

    struct json_value {
        int kind;
        union {
            spargel::base::string string;
            bool boolean;
            struct json_array array;
            struct json_object object;
        };
    };

    struct json_object_entry {
        bool used;
        spargel::base::string key;
        u32 key_hash;
        struct json_value value;
    };

    void json_array_init(struct json_array* array);
    struct json_value* json_array_push(struct json_array* array);
    void json_array_deinit(struct json_array const* array);

    void json_object_init(struct json_object* object);
    struct json_value* json_object_insert(struct json_object* object, spargel::base::string key);
    /**
     * @warning the returned pointer is not stable
     */
    struct json_value* json_object_get(struct json_object* object, spargel::base::string key);
    void json_object_deinit(struct json_object const* object);

    void json_value_deinit(json_value& value);

    enum json_parse_result {
        JSON_PARSE_RESULT_SUCCESS,
        JSON_PARSE_RESULT_UNKNOWN_ERROR,
        JSON_PARSE_RESULT_UNSUPPORTED,
        JSON_PARSE_RESULT_EXPECT_RIGHT_SQUARE,
        JSON_PARSE_RESULT_EXPECT_RIGHT_CURLY,
        JSON_PARSE_RESULT_EXPECT_VALUE,
        JSON_PARSE_RESULT_EXPECT_END_OF_STRING,
        JSON_PARSE_RESULT_EXPECT_TRUE,
        JSON_PARSE_RESULT_EXPECT_FALSE,
        JSON_PARSE_RESULT_EXPECT_COLON,
    };

    int json_parse(char const* str, ssize len, struct json_value* value);

}  // namespace spargel::codec
