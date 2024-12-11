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

    const auto DOT = base::string_from_cstr(".");

    static void normalize_path(spargel::base::string* path) {
        if (path->length() == 0) {
            path->~string();
            *path = DOT;
        } else {
            ssize len = path->length();
            // FIXME
            path->_data = (char*)spargel::base::reallocate(path->_data, len + 1, len + 2,
                                                           spargel::base::ALLOCATION_BASE);
            path->_data[len] = PATH_SPLIT;
            path->_data[len + 1] = '\0';
            path->_length = len + 1;
        }
    }

    void resource_directory_manager_init(struct resource_manager* manager,
                                         spargel::base::string base_path) {
        struct directory_manager_data* data =
            (struct directory_manager_data*)malloc(sizeof(struct directory_manager_data));
        data->base = base_path;
        normalize_path(&data->base);
        manager->data = data;
        manager->op = &directory_manager_operations;
    }

    static void directory_manager_close(struct resource_manager* manager) {
        struct directory_manager_data* data = (struct directory_manager_data*)manager->data;
        data->base.~string();
        free(data);
    }

    static struct resource* directory_manager_open_resource(struct resource_manager* manager,
                                                            struct resource_id id) {
        return NULL;
    }

    struct resource_manager_operations directory_manager_operations = {
        .close = directory_manager_close,
        .open_resource = directory_manager_open_resource,
    };

}  // namespace spargel::resource
