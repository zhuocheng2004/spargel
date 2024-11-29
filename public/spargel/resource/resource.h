#pragma once

#include <spargel/base/base.h>

/* defines */
#define SRESOURCE_DEFAULT_NS "core"

#define SREOURCE_ID_WITH_NS(ns, path)                     \
    ((struct sresource_id){sbase_string_from_literal(ns), \
                           sbase_string_from_literal(path)})

#define SRESOURCE_ID(path) SREOURCE_ID_WITH_NS(SRESOURCE_DEFAULT_NS, path)

/* types */

typedef int sresource_err;

struct sresource_id {
    struct sbase_string ns;
    struct sbase_string path;
};

struct sresource {
    ssize size;
    int ref_cnt;
    struct sresource_operations* op;

    void* data;
};

struct sresource_operations {
    void (*close)(struct sresource*);
    sresource_err (*get_data)(struct sresource*, void* addr);
};

struct sresource_manager {
    struct sresource_manager_operations* op;

    void* data;
};

struct sresource_manager_operations {
    void (*close)(struct sresource_manager*);
    struct sresource* (*open_resource)(struct sresource_manager*,
                                       struct sresource_id id);
};

/* functions */

/* reference +1 */
static inline void sresource_get(struct sresource* resource)
{
    resource->ref_cnt++;
}

/* reference -1 ; close resource if there are no references */
void sresource_put(struct sresource* resource);

static inline ssize sresource_size(struct sresource* resource)
{
    return resource->size;
}

static inline sresource_err sresource_get_data(struct sresource* resource,
                                               void* addr)
{
    return resource->op->get_data(resource, addr);
}

static inline struct sresource* sresource_open_resource(
    struct sresource_manager* manager, struct sresource_id id)
{
    return manager->op->open_resource(manager, id);
}

static inline void sresource_close_manager(struct sresource_manager* manager)
{
    manager->op->close(manager);
}