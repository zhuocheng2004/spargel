#pragma once

#include <spargel/base/types.h>

/* defines */
#define SPGL_RESOURCE_DEFAULT_NS "core"

#define sbase_string_from_literal(str) \
    ((sbase_string){sizeof(str) - 1, str})

#define SPGL_REOURCE_ID_WITH_NS(ns, path)                     \
    ((struct spgl_resource_id){sbase_string_from_literal(ns), \
                               sbase_string_from_literal(path)})

#define SPGL_RESOURCE_ID(path) \
    SPGL_REOURCE_ID_WITH_NS(SPGL_RESOURCE_DEFAULT_NS, path)

/* types */

typedef int spgl_resource_err;

struct spgl_resource_id {
    sbase_string ns;
    sbase_string path;
};

struct spgl_resource {
    ssize size;
    int ref_cnt;
    struct spgl_resource_operations* op;

    void* data;
};

struct spgl_resource_operations {
    void (*close)(struct spgl_resource*);
    spgl_resource_err (*get_data)(struct spgl_resource*, void* addr);
};

struct spgl_resource_manager {
    struct spgl_resource_manager_operations* op;

    void* data;
};

struct spgl_resource_manager_operations {
    void (*close)(struct spgl_resource_manager*);
    struct spgl_resource* (*open_resource)(struct spgl_resource_manager*,
                                           struct spgl_resource_id id);
};

/* functions */

/* reference +1 */
static inline void spgl_resource_get(struct spgl_resource* resource)
{
    resource->ref_cnt++;
}

/* reference -1 ; close resource if there are no references */
void spgl_resource_put(struct spgl_resource* resource);

static inline ssize spgl_resource_size(struct spgl_resource* resource)
{
    return resource->size;
}

static inline spgl_resource_err spgl_resource_get_data(
    struct spgl_resource* resource, void* addr)
{
    return resource->op->get_data(resource, addr);
}

static inline struct spgl_resource* spgl_resource_open_resource(
    struct spgl_resource_manager* manager, struct spgl_resource_id id)
{
    return manager->op->open_resource(manager, id);
}

static inline void spgl_resource_close_manager(
    struct spgl_resource_manager* manager)
{
    manager->op->close(manager);
}
