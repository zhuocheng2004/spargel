#pragma once

#include <spargel/base/base.h>

typedef struct secs_world* secs_world_id;
typedef u64 secs_component_id;
typedef u64 secs_entity_id;

enum secs_result {
    SECS_RESULT_SUCCESS,
    SECS_RESULT_DUPLICATE_COMPONENT,
    SECS_RESULT_INCOMPLETE,
    SECS_RESULT_QUERY_END,
};

struct secs_view {
    u64 archetype_id;
    ssize entity_count;
    secs_entity_id* entities;
    void** components;
};

struct secs_component_descriptor {
    /* ownership is taken by the world */
    struct sbase_string name;
    ssize size;
};

struct secs_spawn_descriptor {
    ssize component_count;
    secs_component_id const* components;
    ssize entity_count;
    // void (*callback)(struct secs_view* view, void* data);
    // void* callback_data;
};

struct secs_query_descriptor {
    u64 start_archetype_id;
    ssize component_count;
    secs_component_id const* components;
    // void (*callback)(struct secs_view* view, void* data);
    // void* callback_data;
};

/**
 * @brief create a new ecs world
 */
secs_world_id secs_create_world();

/**
 * @brief destroy an ecs world
 */
void secs_destroy_world(secs_world_id world);

/**
 * @brief register a component type in an ecs world
 */
int secs_register_component(secs_world_id world,
                            struct secs_component_descriptor const* descriptor,
                            secs_component_id* id);

int secs_spawn_entities(secs_world_id world, struct secs_spawn_descriptor* desc,
                        struct secs_view* view);

int secs_query(secs_world_id world, struct secs_query_descriptor* desc,
               struct secs_view* view);

void secs_delete_entities(secs_world_id world, ssize count,
                          secs_entity_id* entities);
