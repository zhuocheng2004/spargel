#include <spargel/resource/empty.h>
#include <stdlib.h>

struct sresource_manager_operations empty_manager_operations;

void sresource_empty_manager_init(struct sresource_manager* manager)
{
    manager->op = &empty_manager_operations;
}

static struct sresource* empty_manager_open_resource(
    struct sresource_manager* manager, struct sresource_id id)
{
    return NULL;
}

static void empty_manager_close(struct sresource_manager* manager) {}

struct sresource_manager_operations empty_manager_operations = {
    .close = empty_manager_close,
    .open_resource = empty_manager_open_resource,
};
