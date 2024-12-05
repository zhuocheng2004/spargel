#include <spargel/base/base.h>
#include <spargel/ecs/ecs.h>

/* libc */
#include <stdlib.h>
#include <string.h>

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

struct secs_world {
    struct entity_info* entities;
    ssize entity_count;
    ssize entity_capacity;
    struct {
        ssize* sizes;
        ssize count;
        ssize capacity;
    } components;
    struct archetype* archetypes;
    ssize archetype_count;
    ssize archetype_capacity;
};

secs_world_id secs_create_world() {
    struct secs_world* world = sbase_allocate(sizeof(struct secs_world), SBASE_ALLOCATION_ECS);
    memset(world, 0, sizeof(struct secs_world));
    return world;
}

void secs_destroy_world(secs_world_id world) {
    if (!world) return;
    if (world->entities)
        sbase_deallocate(world->entities, sizeof(struct entity_info) * world->entity_capacity,
                         SBASE_ALLOCATION_ECS);
    for (ssize i = 0; i < world->archetype_count; i++) {
        struct archetype* archetype = &world->archetypes[i];
        for (ssize j = 0; j < archetype->row_count; j++) {
            if (archetype->components[j])
                sbase_deallocate(
                    archetype->components[j],
                    world->components.sizes[archetype->component_ids[j]] * archetype->col_capacity,
                    SBASE_ALLOCATION_ECS);
        }
        if (archetype->entities)
            sbase_deallocate(archetype->entities, sizeof(secs_entity_id) * archetype->col_capacity,
                             SBASE_ALLOCATION_ECS);
        if (archetype->component_ids)
            sbase_deallocate(archetype->component_ids,
                             sizeof(secs_component_id) * archetype->row_count,
                             SBASE_ALLOCATION_ECS);
        if (archetype->components)
            sbase_deallocate(archetype->components, sizeof(void*) * archetype->row_count,
                             SBASE_ALLOCATION_ECS);
    }
    if (world->components.sizes)
        sbase_deallocate(world->components.sizes, sizeof(ssize) * world->components.capacity,
                         SBASE_ALLOCATION_ECS);
    if (world->archetypes)
        sbase_deallocate(world->archetypes, sizeof(struct archetype) * world->archetype_capacity,
                         SBASE_ALLOCATION_ECS);
    sbase_deallocate(world, sizeof(struct secs_world), SBASE_ALLOCATION_ECS);
}

/**
 * @brief grow an array
 * @param ptr *ptr points to start of array
 * @param capacity pointer to current capacity
 * @param stride item size
 * @param need the min capacity after growing
 */
static void grow_array(void** ptr, ssize* capacity, ssize stride, ssize need) {
    ssize cap2 = *capacity * 2;
    ssize new_cap = cap2 > need ? cap2 : need;
    if (new_cap < 8) new_cap = 8;
    *ptr = sbase_reallocate(*ptr, *capacity * stride, new_cap * stride, SBASE_ALLOCATION_ECS);
    *capacity = new_cap;
}

int secs_register_component(secs_world_id world, struct secs_component_descriptor const* descriptor,
                            secs_component_id* id) {
    if (world->components.count + 1 > world->components.capacity) {
        grow_array((void**)&world->components.sizes, &world->components.capacity, sizeof(ssize),
                   world->components.count + 1);
    }
    ssize i = world->components.count;
    world->components.count++;
    world->components.sizes[i] = descriptor->size;
    *id = i;
    return SECS_RESULT_SUCCESS;
}

static bool is_subset(ssize count1, secs_component_id const* id1, ssize count2,
                      secs_component_id const* id2) {
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
static ssize find_archetype(secs_world_id world, ssize count, secs_component_id const* ids) {
    ssize result = -1;
    for (ssize i = 0; i < world->archetype_count; i++) {
        struct archetype* archetype = &world->archetypes[i];
        if (archetype->row_count != count) continue;
        if (is_subset(count, ids, archetype->row_count, archetype->component_ids) &&
            is_subset(archetype->row_count, archetype->component_ids, count, ids)) {
            result = i;
            break;
        }
    }
    return result;
}

static ssize create_archetype(secs_world_id world, ssize component_count,
                              secs_component_id const* component_ids) {
    if (world->archetype_count + 1 > world->archetype_capacity) {
        grow_array((void**)&world->archetypes, &world->archetype_capacity, sizeof(struct archetype),
                   world->archetype_count + 1);
    }
    ssize id = world->archetype_count;
    world->archetype_count++;
    struct archetype* archetype = &world->archetypes[id];
    archetype->col_count = 0;
    archetype->col_capacity = 0;
    archetype->entities = NULL;
    archetype->row_count = component_count;
    archetype->component_ids =
        sbase_allocate(sizeof(secs_component_id) * component_count, SBASE_ALLOCATION_ECS);
    memcpy(archetype->component_ids, component_ids, sizeof(secs_component_id) * component_count);
    archetype->components = sbase_allocate(sizeof(void*) * component_count, SBASE_ALLOCATION_ECS);
    memset(archetype->components, 0, sizeof(void*) * component_count);
    return id;
}

int secs_spawn_entities(secs_world_id world, struct secs_spawn_descriptor* desc,
                        struct secs_view* view) {
    ssize archetype_id = find_archetype(world, desc->component_count, desc->components);
    if (archetype_id < 0) {
        archetype_id = create_archetype(world, desc->component_count, desc->components);
    }
    struct archetype* archetype = &world->archetypes[archetype_id];
    if (archetype->col_count + desc->entity_count > archetype->col_capacity) {
        ssize old_capacity = archetype->col_capacity;
        grow_array((void**)&archetype->entities, &archetype->col_capacity, sizeof(secs_entity_id),
                   archetype->col_count + desc->entity_count);
        for (ssize i = 0; i < archetype->row_count; i++) {
            archetype->components[i] = sbase_reallocate(
                archetype->components[i],
                world->components.sizes[archetype->component_ids[i]] * old_capacity,
                world->components.sizes[archetype->component_ids[i]] * archetype->col_capacity,
                SBASE_ALLOCATION_ECS);
        }
    }
    ssize offset = archetype->col_count;
    archetype->col_count += desc->entity_count;

    view->archetype_id = archetype_id;
    view->entity_count = desc->entity_count;

    if (world->entity_count + desc->entity_count > world->entity_capacity) {
        grow_array((void**)&world->entities, &world->entity_capacity, sizeof(struct entity_info),
                   world->entity_count + desc->entity_count);
    }

    view->entities = archetype->entities + offset;
    for (ssize i = 0; i < desc->entity_count; i++) {
        ssize entity = world->entity_count++;
        view->entities[i] = entity;
        world->entities[entity].archetype_id = archetype_id;
        world->entities[entity].index = offset + i;
    }

    for (ssize i = 0; i < desc->component_count; i++) {
        ssize j = 0;
        for (; j < archetype->row_count; j++) {
            if (desc->components[i] == archetype->component_ids[j]) break;
        }
        view->components[i] = (char*)archetype->components[j] +
                              offset * world->components.sizes[archetype->component_ids[j]];
    }
    return SECS_RESULT_SUCCESS;
}

static ssize find_base_archetype(secs_world_id world, ssize component_count,
                                 secs_component_id const* component_ids, u64 start_archetype) {
    ssize i = start_archetype;
    ssize result = -1;
    while (i < world->archetype_count) {
        if (is_subset(component_count, component_ids, world->archetypes[i].row_count,
                      world->archetypes[i].component_ids)) {
            result = i;
            break;
        }
        i++;
    }
    return result;
}

int secs_query(secs_world_id world, struct secs_query_descriptor* desc, struct secs_view* view) {
    ssize archetype_id = find_base_archetype(world, desc->component_count, desc->components,
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

static void delete_in_archetype(secs_world_id world, struct archetype* archetype, ssize index) {
    ssize last = archetype->col_count - 1;
    if (index == last) {
        archetype->col_count--;
        return;
    }
    secs_entity_id last_id = archetype->entities[last];
    archetype->entities[index] = last_id;
    world->entities[last_id].index = index;
    for (ssize i = 0; i < archetype->row_count; i++) {
        ssize size = world->components.sizes[archetype->component_ids[i]];
        memcpy((char*)archetype->components[i] + size * index,
               (char*)archetype->components[i] + size * last, size);
    }
    archetype->col_count--;
}

void secs_delete_entities(secs_world_id world, ssize count, secs_entity_id* entities) {
    for (ssize i = 0; i < count; i++) {
        secs_entity_id id = entities[i];
        ssize archetype_id = world->entities[id].archetype_id;
        ssize index = world->entities[id].index;
        struct archetype* archetype = &world->archetypes[archetype_id];
        delete_in_archetype(world, archetype, index);
    }
}
