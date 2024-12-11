#pragma once

#include <spargel/base/base.h>

/* defines */
#define RESOURCE_DEFAULT_NS "core"

#define REOURCE_ID_WITH_NS(ns, path) \
    ((struct resource_id){sbase_string_from_literal(ns), sbase_string_from_literal(path)})

#define RESOURCE_ID(path) REOURCE_ID_WITH_NS(RESOURCE_DEFAULT_NS, path)

namespace spargel::resource {

    /* types */

    typedef int resource_err;

    struct resource_id {
        struct sbase_string ns;
        struct sbase_string path;
    };

    struct resource {
        ssize size;
        int ref_cnt;
        struct resource_operations* op;

        void* data;
    };

    struct resource_operations {
        void (*close)(struct resource*);
        resource_err (*get_data)(struct resource*, void* addr);
    };

    struct resource_manager {
        struct resource_manager_operations* op;

        void* data;
    };

    struct resource_manager_operations {
        void (*close)(struct resource_manager*);
        struct resource* (*open_resource)(struct resource_manager*, struct resource_id id);
    };

    /* functions */

    /* reference +1 */
    inline void resource_get(struct resource* resource) { resource->ref_cnt++; }

    /* reference -1 ; close resource if there are no references */
    void resource_put(struct resource* resource);

    inline ssize resource_size(struct resource* resource) { return resource->size; }

    inline resource_err resource_get_data(struct resource* resource, void* addr) {
        return resource->op->get_data(resource, addr);
    }

    inline struct resource* resource_open_resource(struct resource_manager* manager,
                                                   struct resource_id id) {
        return manager->op->open_resource(manager, id);
    }

    inline void resource_close_manager(struct resource_manager* manager) {
        manager->op->close(manager);
    }

}  // namespace spargel::resource
