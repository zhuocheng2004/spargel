#include <spargel/codec/codec.h>
#include <spargel/codec/cursor.h>

/* libc */
#include <stdlib.h>
#include <string.h>

namespace spargel::codec {

    /* FNV-1a */
    static u32 hash_string(spargel::base::string str) {
        u32 hash = 2166136261; /* FNV offset base */
        for (ssize i = 0; i < str.length; i++) {
            hash ^= str.data[i];
            hash *= 16777619; /* FNV prime */
        }
        return hash;
    }

    /**
     * @brief find the entry with key
     *
     * if the key exists in the hash map, then return the entry
     * if it does not exist, return an empty entry
     *
     * @warning when the hash map if full, this method will not return
     */
    static struct json_object_entry* find_entry(spargel::base::string key, u32 hash,
                                                struct json_object_entry* entries, ssize capacity);

    /**
     * @brief ensure enough empty space for a hash map
     */
    static void ensure_object_capacity(struct json_object* object);

    /**
     * @brief grow an array
     * @param ptr *ptr points to start of array
     * @param capacity pointer to current capacity
     * @param size item size
     * @param need the min capacity after growing
     */
    static void grow_array(void** ptr, ssize* capacity, ssize size, ssize need) {
        ssize cap2 = *capacity * 2;
        ssize new_cap = cap2 > need ? cap2 : need;
        if (new_cap < 8) new_cap = 8;
        *ptr = realloc(*ptr, new_cap * size);
        *capacity = new_cap;
    }

    struct parser {
        struct cursor cursor;
    };

    static void eat_whitespace(struct parser* ctx) {
        struct cursor* cursor = &ctx->cursor;
        while (!cursor_is_end(cursor)) {
            char ch = cursor_peek(cursor);
            if (ch == ' ' || ch == '\n' || ch == '\r' || ch == '\t')
                cursor_advance(cursor);
            else
                break;
        }
    }

    static int parse_element(struct parser* ctx, struct json_value* value);
    static int parse_value(struct parser* ctx, struct json_value* value);
    static int parse_object(struct parser* ctx, struct json_value* value);
    static int parse_array(struct parser* ctx, struct json_value* value);
    static int parse_string(struct parser* ctx, spargel::base::string* string);
    static int parse_true(struct parser* ctx, struct json_value* value);
    static int parse_false(struct parser* ctx, struct json_value* value);
    static int parse_members(struct parser* ctx, struct json_object* object);
    static int parse_elements(struct parser* ctx, struct json_array* array);

    static int parse_element(struct parser* ctx, struct json_value* value) {
        eat_whitespace(ctx);
        int result = parse_value(ctx, value);
        if (result != JSON_PARSE_RESULT_SUCCESS) return result;
        eat_whitespace(ctx);
        return JSON_PARSE_RESULT_SUCCESS;
    }

    static int parse_value(struct parser* ctx, struct json_value* value) {
        struct cursor* cursor = &ctx->cursor;
        if (cursor_is_end(cursor)) return JSON_PARSE_RESULT_EXPECT_VALUE;
        char ch = cursor_peek(cursor);
        int result;
        switch (ch) {
        case '{':
            /* object */
            result = parse_object(ctx, value);
            break;
        case '[':
            /* array */
            result = parse_array(ctx, value);
            break;
        case '"':
            /* string */
            /* todo: remove this hack */
            value->kind = JSON_VALUE_KIND_STRING;
            result = parse_string(ctx, &value->string);
            break;
        case 't':
            /* true */
            result = parse_true(ctx, value);
            break;
        case 'f':
            /* false */
            result = parse_false(ctx, value);
            break;
        default:
            /* null and number are not supported */
            result = JSON_PARSE_RESULT_UNSUPPORTED;
            break;
        }
        return result;
    }

    static int parse_object(struct parser* ctx, struct json_value* value) {
        struct cursor* cursor = &ctx->cursor;
        cursor_advance(cursor); /* { */
        value->kind = JSON_VALUE_KIND_OBJECT;
        json_object_init(&value->object);
        eat_whitespace(ctx);
        if (cursor_is_end(cursor)) return JSON_PARSE_RESULT_EXPECT_RIGHT_CURLY;
        if (cursor_try_eat_char(cursor, '}')) return JSON_PARSE_RESULT_SUCCESS;
        int result = parse_members(ctx, &value->object);
        if (result != JSON_PARSE_RESULT_SUCCESS) return result;
        if (!cursor_try_eat_char(cursor, '}')) return JSON_PARSE_RESULT_EXPECT_RIGHT_CURLY;
        return JSON_PARSE_RESULT_SUCCESS;
    }

    static int parse_array(struct parser* ctx, struct json_value* value) {
        struct cursor* cursor = &ctx->cursor;
        cursor_advance(cursor); /* [ */
        value->kind = JSON_VALUE_KIND_ARRAY;
        json_array_init(&value->array);
        if (cursor_is_end(cursor)) return JSON_PARSE_RESULT_EXPECT_RIGHT_SQUARE;
        if (cursor_try_eat_char(cursor, ']')) return JSON_PARSE_RESULT_SUCCESS;
        int result = parse_elements(ctx, &value->array);
        if (result != JSON_PARSE_RESULT_SUCCESS) return result;
        if (!cursor_try_eat_char(cursor, ']')) return JSON_PARSE_RESULT_EXPECT_RIGHT_SQUARE;
        return JSON_PARSE_RESULT_SUCCESS;
    }

    static int parse_string(struct parser* ctx, spargel::base::string* string) {
        struct cursor* cursor = &ctx->cursor;
        cursor_advance(cursor); /* " */
        if (cursor_is_end(cursor)) return JSON_PARSE_RESULT_EXPECT_END_OF_STRING;
        char const* begin = cursor->cur;
        char const* end;
        while (!cursor_is_end(cursor)) {
            char ch = cursor_peek(cursor);
            if (ch == '"') {
                end = cursor->cur;
                break;
            }
            cursor_advance(cursor);
        }
        if (cursor_is_end(cursor)) return JSON_PARSE_RESULT_EXPECT_END_OF_STRING;
        cursor_advance(cursor);
        *string = spargel::base::string_from_range(begin, end);
        return JSON_PARSE_RESULT_SUCCESS;
    }

    static int parse_true(struct parser* ctx, struct json_value* value) {
        struct cursor* cursor = &ctx->cursor;
        if (!cursor_try_eat_bytes(cursor, (u8*)"true", 4)) return JSON_PARSE_RESULT_EXPECT_TRUE;
        value->kind = JSON_VALUE_KIND_BOOLEAN;
        value->boolean = true;
        return JSON_PARSE_RESULT_SUCCESS;
    }

    static int parse_false(struct parser* ctx, struct json_value* value) {
        struct cursor* cursor = &ctx->cursor;
        if (!cursor_try_eat_bytes(cursor, (u8*)"false", 5)) return JSON_PARSE_RESULT_EXPECT_FALSE;
        value->kind = JSON_VALUE_KIND_BOOLEAN;
        value->boolean = true;
        return JSON_PARSE_RESULT_SUCCESS;
    }

    static int parse_members(struct parser* ctx, struct json_object* object) {
        struct cursor* cursor = &ctx->cursor;
        json_object_init(object);
        int result = JSON_PARSE_RESULT_UNKNOWN_ERROR;
        while (!cursor_is_end(cursor)) {
            spargel::base::string key;
            eat_whitespace(ctx);
            if (cursor_is_end(cursor)) break;
            if (cursor_peek(cursor) != '"') break;
            result = parse_string(ctx, &key);
            if (result != JSON_PARSE_RESULT_SUCCESS) break;
            eat_whitespace(ctx);
            if (!cursor_try_eat_char(cursor, ':')) {
                result = JSON_PARSE_RESULT_EXPECT_COLON;
                break;
            }
            /* get an entry with key */
            struct json_value* value = json_object_insert(object, key);
            result = parse_element(ctx, value);
            if (result != JSON_PARSE_RESULT_SUCCESS) break;
            bool has_next = cursor_try_eat_char(cursor, ',');
            if (!has_next) {
                result = JSON_PARSE_RESULT_SUCCESS;
                break;
            }
            result = JSON_PARSE_RESULT_UNKNOWN_ERROR;
        }
        return result;
    }

    static int parse_elements(struct parser* ctx, struct json_array* array) {
        struct cursor* cursor = &ctx->cursor;
        json_array_init(array);
        int result = JSON_PARSE_RESULT_UNKNOWN_ERROR;
        while (!cursor_is_end(cursor)) {
            struct json_value* value = json_array_push(array);
            result = parse_element(ctx, value);
            if (result != JSON_PARSE_RESULT_SUCCESS) break;
            bool has_next = cursor_try_eat_char(cursor, ',');
            if (!has_next) {
                result = JSON_PARSE_RESULT_SUCCESS;
                break;
            }
            result = JSON_PARSE_RESULT_UNKNOWN_ERROR;
        }
        return result;
    }

    int json_parse(char const* str, ssize len, struct json_value* value) {
        struct parser ctx;
        ctx.cursor.cur = str;
        ctx.cursor.end = str + len;
        return parse_element(&ctx, value);
    }

    static struct json_object_entry* find_entry(spargel::base::string key, u32 hash,
                                                struct json_object_entry* entries, ssize capacity) {
        ssize index = hash % capacity;
        for (;;) {
            struct json_object_entry* entry = &entries[index];
            if (!entry->used || key == entry->key) {
                return entry;
            }
            index = (index + 1) % capacity;
        }
    }

    static void ensure_object_capacity(struct json_object* object) {
        // if (object->count < object->capacity / 8 * 7) return;
        /* need to grow */
        ssize safe_capacity = object->capacity * 2;
        safe_capacity = safe_capacity < 8 ? 8 : safe_capacity;
        struct json_object_entry* entries =
            (struct json_object_entry*)malloc(sizeof(struct json_object_entry) * safe_capacity);
        memset(entries, 0, sizeof(struct json_object_entry) * safe_capacity);
        for (ssize i = 0; i < object->capacity; i++) {
            struct json_object_entry* entry = &object->entries[i];
            struct json_object_entry* dest =
                find_entry(entry->key, entry->key_hash, entries, safe_capacity);
            memcpy(dest, entry, sizeof(struct json_object_entry));
        }
        if (object->entries) free(object->entries);
        object->entries = entries;
        object->capacity = safe_capacity;
    }

    void json_object_init(struct json_object* object) {
        object->entries = NULL;
        object->count = 0;
        object->capacity = 0;
    }

    struct json_value* json_object_insert(struct json_object* object, spargel::base::string key) {
        ensure_object_capacity(object);
        u32 hash = hash_string(key);
        struct json_object_entry* entry = find_entry(key, hash, object->entries, object->capacity);
        if (!entry->used) {
            entry->used = true;
            entry->key = key;
            entry->key_hash = hash;
            object->count++;
        }
        return &entry->value;
    }

    struct json_value* json_object_get(struct json_object* object, spargel::base::string key) {
        // ensure_object_capacity(object);
        u32 hash = hash_string(key);
        struct json_object_entry* entry = find_entry(key, hash, object->entries, object->capacity);
        if (entry->used) return &entry->value;
        return NULL;
    }

    void json_object_deinit(struct json_object const* object) {
        for (ssize i = 0; i < object->capacity; i++) {
            struct json_object_entry* entry = &object->entries[i];
            if (entry->used) {
                spargel::base::destroy(entry->key);
                json_value_deinit(&entry->value);
            }
        }
        if (object->entries) free(object->entries);
    }

    void json_array_init(struct json_array* array) {
        array->values = NULL;
        array->count = 0;
        array->capacity = 0;
    }

    struct json_value* json_array_push(struct json_array* array) {
        if (array->count + 1 > array->capacity) {
            grow_array((void**)&array->values, &array->capacity, sizeof(struct json_value),
                       array->count + 1);
        }
        struct json_value* value = &array->values[array->count];
        array->count++;
        return value;
    }

    void json_array_deinit(struct json_array const* array) {
        for (ssize i = 0; i < array->count; i++) {
            json_value_deinit(&array->values[i]);
        }
        if (array->values) free(array->values);
    }

    void json_value_deinit(struct json_value const* value) {
        switch (value->kind) {
        case JSON_VALUE_KIND_ARRAY:
            json_array_deinit(&value->array);
            break;
        case JSON_VALUE_KIND_OBJECT:
            json_object_deinit(&value->object);
            break;
        case JSON_VALUE_KIND_STRING:
            spargel::base::destroy(value->string);
            break;
        default:
            break;
        }
    }

}  // namespace spargel::codec
