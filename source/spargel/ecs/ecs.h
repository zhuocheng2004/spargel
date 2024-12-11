#pragma once

#include <spargel/base/types.h>

namespace spargel::ecs {

    typedef struct world* world_id;
    typedef u64 component_id;
    typedef u64 entity_id;

    enum result {
        RESULT_SUCCESS,
        RESULT_DUPLICATE_COMPONENT,
        RESULT_INCOMPLETE,
        RESULT_QUERY_END,
    };

    struct view {
        u64 archetype_id;
        ssize entity_count;
        entity_id* entities;
        void** components;
    };

    struct component_descriptor {
        ssize size;
    };

    struct spawn_descriptor {
        ssize component_count;
        component_id const* components;
        ssize entity_count;
    };

    struct query_descriptor {
        u64 start_archetype_id;
        ssize component_count;
        component_id const* components;
    };

    /**
     * @brief create a new ecs world
     */
    world_id create_world();

    /**
     * @brief destroy an ecs world
     */
    void destroy_world(world_id world);

    /**
     * @brief register a component type in an ecs world
     */
    int register_component(world_id world,
                                struct component_descriptor const* descriptor,
                                component_id* id);

    int spawn_entities(world_id world, struct spawn_descriptor* desc,
                            struct view* view);

    int query(world_id world, struct query_descriptor* desc, struct view* view);

    void delete_entities(world_id world, ssize count, entity_id* entities);

}  // namespace spargel::ecs
