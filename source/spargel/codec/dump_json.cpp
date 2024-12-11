#include <spargel/codec/codec.h>

/* libc */
#include <stdio.h>

static void dump_json_value(struct scodec_json_value const* value);

static void dump_json_array(struct scodec_json_array const* array) {
    printf("[");
    for (ssize i = 0; i < array->count; i++) {
        dump_json_value(&array->values[i]);
        if (i < array->count - 1) printf(",");
    }
    printf("]");
}
static void dump_json_object(struct scodec_json_object const* object) {
    printf("{");
    int cnt = 0;
    for (ssize i = 0; i < object->capacity; i++) {
        struct scodec_json_object_entry* entry = &object->entries[i];
        if (entry->used) {
            printf("\"%s\":", entry->key.data);
            dump_json_value(&entry->value);
            cnt++;
            if (cnt < object->count) printf(",");
        }
    }
    printf("}");
}

static void dump_json_value(struct scodec_json_value const* value) {
    switch (value->kind) {
    case SCODEC_JSON_VALUE_KIND_ARRAY:
        dump_json_array(&value->array);
        break;
    case SCODEC_JSON_VALUE_KIND_OBJECT:
        dump_json_object(&value->object);
        break;
    case SCODEC_JSON_VALUE_KIND_STRING:
        printf("\"%s\"", value->string.data);
        break;
    case SCODEC_JSON_VALUE_KIND_BOOLEAN:
        if (value->boolean)
            printf("true");
        else
            printf("false");
        break;
    default:
        break;
    }
}

struct file {
    char* data;
    ssize length;
};

/* todo: error checking */
static void read_file(char const* path, struct file* f) {
    FILE* file = fopen(path, "rb");
    fseek(file, 0, SEEK_END);
    ssize len = ftell(file);
    char* data = (char*)sbase_allocate(len, SBASE_ALLOCATION_CODEC);
    fseek(file, 0, SEEK_SET);
    fread(data, len, 1, file);
    // data[len] = 0;
    f->data = data;
    f->length = len;
}

int main(int argc, char* argv[]) {
    if (argc < 2) return -1;
    struct file f;
    read_file(argv[1], &f);

    struct scodec_json_value value;
    int result = scodec_json_parse(f.data, f.length, &value);

    dump_json_value(&value);

    sbase_deallocate(f.data, f.length, SBASE_ALLOCATION_CODEC);

    scodec_json_value_deinit(&value);

    sbase_report_allocation();
    sbase_check_leak();
    return result;
}
