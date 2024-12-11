#include <spargel/base/base.h>
#include <spargel/base/const.h>
#include <spargel/resource/directory.h>

/* libc */
#include <stdlib.h>

namespace spargel::resource {

    extern struct resource_manager_operations directory_manager_operations;

    struct directory_manager_data {
        spargel::base::string base;
    };

    const spargel::base::string DOT = string_from_literal(".");

    static void normalize_path(spargel::base::string* path) {
        if (path->length == 0) {
            spargel::base::string_deinit(*path);
            spargel::base::string_copy(path, DOT);
        } else {
            ssize len = path->length;
            // FIXME
            path->data =
                (char*)spargel::base::reallocate(path->data, len + 1, len + 2, spargel::base::ALLOCATION_BASE);
            path->data[len] = PATH_SPLIT;
            path->data[len + 1] = '\0';
            path->length = len + 1;
        }
    }

    void resource_directory_manager_init(struct resource_manager* manager,
                                              spargel::base::string base_path) {
        struct directory_manager_data* data =
            (struct directory_manager_data*)malloc(sizeof(struct directory_manager_data));
        data->base.data = NULL;
        spargel::base::string_copy(&data->base, base_path);
        normalize_path(&data->base);
        manager->data = data;
        manager->op = &directory_manager_operations;
    }

    static void directory_manager_close(struct resource_manager* manager) {
        struct directory_manager_data* data = (struct directory_manager_data*)manager->data;
        spargel::base::string_deinit(data->base);
        free(data);
    }

    static struct resource* directory_manager_open_resource(
        struct resource_manager* manager, struct resource_id id) {
        return NULL;
    }

    struct resource_manager_operations directory_manager_operations = {
        .close = directory_manager_close,
        .open_resource = directory_manager_open_resource,
    };

}  // namespace spargel::resource
