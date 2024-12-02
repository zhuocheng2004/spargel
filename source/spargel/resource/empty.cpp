#include <stdlib.h>

import spargel.base.c;

#include <spargel/resource/empty.h>

extern struct spgl_resource_manager_operations empty_manager_operations;

void spgl_resource_empty_manager_init(struct spgl_resource_manager* manager)
{
    manager->op = &empty_manager_operations;
}

static struct spgl_resource* empty_manager_open_resource(
    struct spgl_resource_manager* manager, struct spgl_resource_id id)
{
    return NULL;
}

static void empty_manager_close(struct spgl_resource_manager* manager) {}

struct spgl_resource_manager_operations empty_manager_operations = {
    .close = empty_manager_close,
    .open_resource = empty_manager_open_resource,
};
