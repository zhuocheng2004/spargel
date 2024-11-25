#include <spargel/codec/codec.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// static char const* json_data =
//     "{"
//     "\"abc\": \"def\","
//     "\"x123\": [ true, false ],"
//     "\"y456\": { \"a\": \"b\", \"c\": \"d\" }"
//     "}";

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

/* todo: error checking */
static char* read_file(char const* path) {
  FILE* file = fopen(path, "rb");
  fseek(file, 0, SEEK_END);
  ssize len = ftell(file);
  char* data = malloc(len + 1);
  fseek(file, 0, SEEK_SET);
  fread(data, len, 1, file);
  data[len] = 0;
  return data;
}

int main(int argc, char* argv[]) {
  if (argc < 2) return -1;
  char* json_data = read_file(argv[1]);

  struct scodec_json_value value;
  int result = scodec_json_parse(json_data, strlen(json_data), &value);
  // if (result != SCODEC_JSON_PARSE_RESULT_SUCCESS) {
  //   printf("error: parse failed\n");
  //   return 1;
  // }

  free(json_data);

  dump_json_value(&value);

  scodec_json_value_deinit(&value);
  return result;
}
