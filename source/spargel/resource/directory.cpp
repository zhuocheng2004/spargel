#include <stdlib.h>

// #include <spargel/base/base.h>
#include <spargel/base/const.h>

import spargel.base.c;

#include <spargel/resource/directory.h>

extern struct spgl_resource_manager_operations directory_manager_operations;

struct directory_manager_data {
    sbase_string base;
};

const sbase_string DOT = sbase_string_from_literal(".");

static void normalize_path(sbase_string* path)
{
    if (path->length == 0) {
        sbase_string_deinit(*path);
        sbase_string_copy(path, DOT);
    } else {
        ssize len = path->length;
        // FIXME
        path->data = (char*)realloc(path->data, len + 2);
        path->data[len] = PATH_SPLIT;
        path->data[len + 1] = '\0';
        path->length = len + 1;
    }
}

void spgl_resource_directory_manager_init(struct spgl_resource_manager* manager,
                                           sbase_string base_path)
{
    struct directory_manager_data* data =
        (struct directory_manager_data*)malloc(
            sizeof(struct directory_manager_data));
    data->base.data = NULL;
    sbase_string_copy(&data->base, base_path);
    normalize_path(&data->base);
    manager->data = data;
    manager->op = &directory_manager_operations;
}

static void directory_manager_close(struct spgl_resource_manager* manager)
{
    struct directory_manager_data* data =
        (struct directory_manager_data*)manager->data;
    sbase_string_deinit(data->base);
    free(data);
}

static struct spgl_resource* directory_manager_open_resource(
    struct spgl_resource_manager* manager, struct spgl_resource_id id)
{
    return NULL;
}

struct spgl_resource_manager_operations directory_manager_operations = {
    .close = directory_manager_close,
    .open_resource = directory_manager_open_resource,
};
