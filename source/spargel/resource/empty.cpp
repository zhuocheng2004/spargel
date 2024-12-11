#include <spargel/resource/empty.h>

/* libc */
#include <stdlib.h>

namespace spargel::resource {

    extern struct resource_manager_operations empty_manager_operations;

    void resource_empty_manager_init(struct resource_manager* manager) {
        manager->op = &empty_manager_operations;
    }

    static struct resource* empty_manager_open_resource(struct resource_manager* manager,
                                                             struct resource_id id) {
        return NULL;
    }

    static void empty_manager_close(struct resource_manager* manager) {}

    struct resource_manager_operations empty_manager_operations = {
        .close = empty_manager_close,
        .open_resource = empty_manager_open_resource,
    };

}  // namespace spargel::resource
