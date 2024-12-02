#include <spargel/ecs/ecs.h>
#include <stdio.h>
#include <stdlib.h>

struct position {
    float x;
};

struct velocity {
    float v;
};

struct health {
    float h;
};

struct delete_queue {
    secs_entity_id* ids;
    ssize count;
    ssize capacity;
};

/**
 * @brief grow an array
 * @param ptr *ptr points to start of array
 * @param capacity pointer to current capacity
 * @param stride item size
 * @param need the min capacity after growing
 */
static void grow_array(void** ptr, ssize* capacity, ssize stride, ssize need)
{
    ssize cap2 = *capacity * 2;
    ssize new_cap = cap2 > need ? cap2 : need;
    if (new_cap < 8) new_cap = 8;
    *ptr = realloc(*ptr, new_cap * stride);
    *capacity = new_cap;
}

static void delete_queue_push(struct delete_queue* queue, secs_entity_id id)
{
    if (queue->count + 1 > queue->capacity) {
        grow_array((void**)&queue->ids, &queue->capacity,
                   sizeof(secs_entity_id), queue->count + 1);
    }
    queue->ids[queue->count] = id;
    queue->count++;
}

static void destroy_delete_queue(struct delete_queue* queue)
{
    if (queue->ids) free(queue->ids);
}

int main()
{
    secs_world_id world = secs_create_world();

    secs_component_id position_id;
    secs_component_id velocity_id;
    secs_component_id health_id;
    secs_register_component(
        world,
        &(struct secs_component_descriptor){.size = sizeof(struct position)},
        &position_id);
    secs_register_component(
        world,
        &(struct secs_component_descriptor){.size = sizeof(struct velocity)},
        &velocity_id);
    secs_register_component(
        world,
        &(struct secs_component_descriptor){.size = sizeof(struct health)},
        &health_id);

    void* components[3];
    struct secs_view view = {.components = components};
    {
        secs_component_id ids[] = {position_id};
        secs_spawn_entities(
            world,
            &(struct secs_spawn_descriptor){
                .component_count = 1, .components = ids, .entity_count = 10},
            &view);
        printf("info: spawned 10 x [ position ] with archetype %llu\n",
               view.archetype_id);
        struct position* pos = components[0];
        for (ssize i = 0; i < view.entity_count; i++) {
            pos[i].x = i;
        }
    }
    {
        secs_component_id ids[] = {position_id, velocity_id};
        secs_spawn_entities(
            world,
            &(struct secs_spawn_descriptor){
                .component_count = 2, .components = ids, .entity_count = 20},
            &view);
        printf(
            "info: spawned 20 x [ position, velocity ] with archetype "
            "%llu\n",
            view.archetype_id);
        struct position* pos = components[0];
        struct velocity* vel = components[1];
        for (ssize i = 0; i < view.entity_count; i++) {
            pos[i].x = -i;
            vel[i].v = i;
        }
    }
    {
        secs_component_id ids[] = {position_id, health_id, velocity_id};
        secs_spawn_entities(
            world,
            &(struct secs_spawn_descriptor){
                .component_count = 3, .components = ids, .entity_count = 50},
            &view);
        printf(
            "info: spawned 50 x [ position, health, velocity ] with archetype "
            "%llu\n",
            view.archetype_id);
        struct position* pos = components[0];
        struct health* health = components[1];
        struct velocity* vel = components[2];
        for (ssize i = 0; i < view.entity_count; i++) {
            pos[i].x = -i;
            health[i].h = 100;
            vel[i].v = i;
        }
    }
    {
        secs_component_id ids[] = {velocity_id};
        secs_spawn_entities(
            world,
            &(struct secs_spawn_descriptor){
                .component_count = 1, .components = ids, .entity_count = 5},
            &view);
        printf("info: spawned 5 x [ velocity ] with archetype %llu\n",
               view.archetype_id);
    }

    {
        secs_component_id ids[] = {position_id};
        u64 archetype_id = 0;
        while (true) {
            int result = secs_query(world,
                                    &(struct secs_query_descriptor){
                                        .start_archetype_id = archetype_id,
                                        .component_count = 1,
                                        .components = ids,
                                    },
                                    &view);
            if (result == SECS_RESULT_QUERY_END) break;
            printf(
                "info: query: get %td entities with [ position ] in archetype "
                "%llu\n",
                view.entity_count, view.archetype_id);
            archetype_id = view.archetype_id + 1;

            struct position* pos = components[0];
            for (ssize i = 0; i < view.entity_count; i++) {
                printf("info: query:  - entity %llu [ position = %.2f ]\n",
                       view.entities[i], pos[i].x);
            }
        }
    }

    {
        secs_component_id ids[] = {health_id};
        u64 archetype_id = 0;
        while (true) {
            int result = secs_query(world,
                                    &(struct secs_query_descriptor){
                                        .start_archetype_id = archetype_id,
                                        .component_count = 1,
                                        .components = ids,
                                    },
                                    &view);
            if (result == SECS_RESULT_QUERY_END) break;
            printf(
                "info: query: get %td entities with [ health ] in archetype "
                "%llu\n",
                view.entity_count, view.archetype_id);
            archetype_id = view.archetype_id + 1;

            struct health* health = components[0];
            for (ssize i = 0; i < view.entity_count; i++) {
                printf("info: query:  - entity %llu [ health = %.2f ]\n",
                       view.entities[i], health[i].h);
            }
        }
    }

    {
        secs_component_id ids[] = {position_id, velocity_id};
        secs_spawn_entities(
            world,
            &(struct secs_spawn_descriptor){
                .component_count = 2, .components = ids, .entity_count = 2000},
            &view);
        printf(
            "info: spawned 2000 x [ position, velocity ] with archetype "
            "%llu\n",
            view.archetype_id);
        struct position* pos = components[0];
        struct velocity* vel = components[1];
        for (ssize i = 0; i < view.entity_count; i++) {
            pos[i].x = -i;
            vel[i].v = i;
        }
    }

    struct delete_queue queue = {};

    {
        secs_component_id ids[] = {position_id};
        u64 archetype_id = 0;
        while (true) {
            int result = secs_query(world,
                                    &(struct secs_query_descriptor){
                                        .start_archetype_id = archetype_id,
                                        .component_count = 1,
                                        .components = ids,
                                    },
                                    &view);
            if (result == SECS_RESULT_QUERY_END) break;
            printf(
                "info: query: get %td entities with [ position ] in archetype "
                "%llu\n",
                view.entity_count, view.archetype_id);
            archetype_id = view.archetype_id + 1;

            struct position* pos = components[0];
            for (ssize i = 0; i < view.entity_count; i++) {
                if (pos[i].x >= 5 || pos[i].x <= -5) {
                    delete_queue_push(&queue, view.entities[i]);
                }
            }
        }
    }

    secs_delete_entities(world, queue.count, queue.ids);
    printf("info: deleted %td entities\n", queue.count);

    {
        u64 archetype_id = 0;
        while (true) {
            int result =
                secs_query(world,
                           &(struct secs_query_descriptor){
                               .start_archetype_id = archetype_id,
                               .component_count = 1,
                               .components = (secs_component_id[]){position_id},
                           },
                           &view);
            if (result == SECS_RESULT_QUERY_END) break;
            printf(
                "info: query: get %td entities with [ position ] in archetype "
                "%llu\n",
                view.entity_count, view.archetype_id);
            archetype_id = view.archetype_id + 1;

            struct position* pos = components[0];
            for (ssize i = 0; i < view.entity_count; i++) {
                printf("info: query:  - entity %llu [ position = %.2f ]\n",
                       view.entities[i], pos[i].x);
            }
        }
    }

    destroy_delete_queue(&queue);
    secs_destroy_world(world);
    return 0;
}
