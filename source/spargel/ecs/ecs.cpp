module;

#include <stdlib.h>
#include <string.h>

#include <spargel/base/types.h>

module spargel.ecs;

import spargel.base.allocator;
import spargel.base.c;
import spargel.base.container;
import spargel.base.memory;

struct archetype {
    ssize col_count;
    ssize col_capacity;
    secs_entity_id* entities;
    ssize row_count;
    secs_component_id* component_ids;
    void** components;
};

struct entity_info {
    ssize archetype_id;
    ssize index;
};

struct component_info {
    ssize size;
};

struct secs_world {
    spargel::base::vector<entity_info> entities;
    spargel::base::vector<component_info> components;
    spargel::base::vector<archetype> archetypes;
};

secs_world_id secs_create_world()
{
    secs_world* world = (secs_world*)spargel::base::default_allocator{}.allocate(sizeof(secs_world));
    spargel::base::construct_at<secs_world>(world);
    return world;
}

void secs_destroy_world(secs_world_id world)
{
    if (!world) return;
    for (ssize i = 0; i < world->archetypes.count(); i++) {
        struct archetype* archetype = &world->archetypes[i];
        if (archetype->entities) free(archetype->entities);
        if (archetype->component_ids) free(archetype->component_ids);
        for (ssize j = 0; j < archetype->row_count; j++) {
            if (archetype->components[j]) free(archetype->components[j]);
        }
    }
    spargel::base::destroy_at(world);
    spargel::base::default_allocator{}.deallocate(world, sizeof(secs_world));
}

static void grow_array(void** ptr, ssize* capacity, ssize stride, ssize need)
{
    ssize cap2 = *capacity * 2;
    ssize new_cap = cap2 > need ? cap2 : need;
    if (new_cap < 8) new_cap = 8;
    *ptr = realloc(*ptr, new_cap * stride);
    *capacity = new_cap;
}

int secs_register_component(secs_world_id world,
                            struct secs_component_descriptor const* descriptor,
                            secs_component_id* id)
{
    ssize i = world->components.count();
    world->components.push_back(descriptor->size);
    *id = i;
    return SECS_RESULT_SUCCESS;
}

static bool is_subset(ssize count1, secs_component_id const* id1, ssize count2,
                      secs_component_id const* id2)
{
    if (count1 > count2) return false;
    for (ssize i = 0; i < count1; i++) {
        bool found = false;
        for (ssize j = 0; j < count2; j++) {
            if (id1[i] == id2[j]) {
                found = true;
                break;
            }
        }
        if (!found) return false;
    }
    return true;
}

/**
 * @brief find the archetype with the given set of components
 */
static ssize find_archetype(secs_world_id world, ssize count,
                            secs_component_id const* ids)
{
    ssize result = -1;
    for (ssize i = 0; i < world->archetypes.count(); i++) {
        struct archetype* archetype = &world->archetypes[i];
        if (archetype->row_count != count) continue;
        if (is_subset(count, ids, archetype->row_count,
                      archetype->component_ids) &&
            is_subset(archetype->row_count, archetype->component_ids, count,
                      ids)) {
            result = i;
            break;
        }
    }
    return result;
}

static ssize create_archetype(secs_world_id world, ssize component_count,
                              secs_component_id const* component_ids)
{
    ssize id = world->archetypes.count();
    auto component_ids_copy =
        (secs_component_id*)malloc(sizeof(secs_component_id) * component_count);
    memcpy(component_ids_copy, component_ids,
           sizeof(secs_component_id) * component_count);
    auto components = (void**)malloc(sizeof(void*) * component_count);
    memset(components, 0, sizeof(void*) * component_count);
    world->archetypes.push_back(0, 0, nullptr, component_count, component_ids_copy, components);
    return id;
}

int secs_spawn_entities(secs_world_id world, struct secs_spawn_descriptor* desc,
                        struct secs_view* view)
{
    ssize archetype_id =
        find_archetype(world, desc->component_count, desc->components);
    if (archetype_id < 0) {
        archetype_id =
            create_archetype(world, desc->component_count, desc->components);
    }
    struct archetype* archetype = &world->archetypes[archetype_id];
    if (archetype->col_count + desc->entity_count > archetype->col_capacity) {
        grow_array((void**)&archetype->entities, &archetype->col_capacity,
                   sizeof(secs_entity_id),
                   archetype->col_count + desc->entity_count);
        for (ssize i = 0; i < archetype->row_count; i++) {
            archetype->components[i] =
                realloc(archetype->components[i],
                        world->components[archetype->component_ids[i]].size *
                            archetype->col_capacity);
        }
    }
    ssize offset = archetype->col_count;
    archetype->col_count += desc->entity_count;

    view->archetype_id = archetype_id;
    view->entity_count = desc->entity_count;

    world->entities.reserve(world->entities.count() + desc->entity_count);

    view->entities = archetype->entities + offset;
    for (ssize i = 0; i < desc->entity_count; i++) {
        ssize entity = world->entities.count();
        view->entities[i] = entity;
        world->entities.push_back(archetype_id, offset + i);
    }

    for (ssize i = 0; i < desc->component_count; i++) {
        ssize j = 0;
        for (; j < archetype->row_count; j++) {
            if (desc->components[i] == archetype->component_ids[j]) break;
        }
        view->components[i] =
            (char*)archetype->components[j] +
            offset * world->components[archetype->component_ids[j]].size;
    }
    return SECS_RESULT_SUCCESS;
}

static ssize find_base_archetype(secs_world_id world, ssize component_count,
                                 secs_component_id const* component_ids,
                                 u64 start_archetype)
{
    ssize i = start_archetype;
    ssize result = -1;
    while (i < world->archetypes.count()) {
        if (is_subset(component_count, component_ids,
                      world->archetypes[i].row_count,
                      world->archetypes[i].component_ids)) {
            result = i;
            break;
        }
        i++;
    }
    return result;
}

int secs_query(secs_world_id world, struct secs_query_descriptor* desc,
               struct secs_view* view)
{
    ssize archetype_id =
        find_base_archetype(world, desc->component_count, desc->components,
                            desc->start_archetype_id);
    if (archetype_id < 0) return SECS_RESULT_QUERY_END;

    struct archetype* archetype = &world->archetypes[archetype_id];

    view->archetype_id = archetype_id;
    view->entity_count = archetype->col_count;
    view->entities = archetype->entities;
    for (ssize i = 0; i < desc->component_count; i++) {
        ssize j = 0;
        for (; j < archetype->row_count; j++) {
            if (desc->components[i] == archetype->component_ids[j]) break;
        }
        view->components[i] = (char*)archetype->components[j];
    }

    return SECS_RESULT_INCOMPLETE;
}

static void delete_in_archetype(secs_world_id world,
                                struct archetype* archetype, ssize index)
{
    ssize last = archetype->col_count - 1;
    if (index == last) {
        archetype->col_count--;
        return;
    }
    secs_entity_id last_id = archetype->entities[last];
    archetype->entities[index] = last_id;
    world->entities[last_id].index = index;
    for (ssize i = 0; i < archetype->row_count; i++) {
        ssize size = world->components[archetype->component_ids[i]].size;
        memcpy((char*)archetype->components[i] + size * index,
               (char*)archetype->components[i] + size * last, size);
    }
    archetype->col_count--;
}

void secs_delete_entities(secs_world_id world, ssize count,
                          secs_entity_id* entities)
{
    for (ssize i = 0; i < count; i++) {
        secs_entity_id id = entities[i];
        ssize archetype_id = world->entities[id].archetype_id;
        ssize index = world->entities[id].index;
        struct archetype* archetype = &world->archetypes[archetype_id];
        delete_in_archetype(world, archetype, index);
    }
}
