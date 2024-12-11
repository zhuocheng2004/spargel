#include <spargel/base/base.h>
#include <spargel/ecs/ecs.h>

/* libc */
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
    spargel::ecs::entity_id* ids;
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
static void grow_array(void** ptr, ssize* capacity, ssize stride, ssize need) {
    ssize cap2 = *capacity * 2;
    ssize new_cap = cap2 > need ? cap2 : need;
    if (new_cap < 8) new_cap = 8;
    *ptr = sbase_reallocate(*ptr, *capacity * stride, new_cap * stride, SBASE_ALLOCATION_ECS);
    *capacity = new_cap;
}

static void delete_queue_push(struct delete_queue* queue, spargel::ecs::entity_id id) {
    if (queue->count + 1 > queue->capacity) {
        grow_array((void**)&queue->ids, &queue->capacity, sizeof(spargel::ecs::entity_id),
                   queue->count + 1);
    }
    queue->ids[queue->count] = id;
    queue->count++;
}

static void destroy_delete_queue(struct delete_queue* queue) {
    if (queue->ids)
        sbase_deallocate(queue->ids, sizeof(ssize) * queue->capacity, SBASE_ALLOCATION_ECS);
}

int main() {
    spargel::ecs::world_id world = spargel::ecs::create_world();

    spargel::ecs::component_id position_id;
    spargel::ecs::component_id velocity_id;
    spargel::ecs::component_id health_id;
    {
        struct spargel::ecs::component_descriptor desc;

        desc.size = sizeof(struct position);
        spargel::ecs::register_component(world, &desc, &position_id);

        desc.size = sizeof(struct velocity);
        spargel::ecs::register_component(world, &desc, &velocity_id);

        desc.size = sizeof(struct health);
        spargel::ecs::register_component(world, &desc, &health_id);
    }

    void* components[3];
    struct spargel::ecs::view view = {.components = components};
    {
        spargel::ecs::component_id ids[] = {position_id};
        struct spargel::ecs::spawn_descriptor desc = {
            .component_count = 1,
            .components = ids,
            .entity_count = 10,
        };
        spargel::ecs::spawn_entities(world, &desc, &view);
        printf("info: spawned 10 x [ position ] with archetype %lu\n", view.archetype_id);
        struct position* pos = (struct position*)components[0];
        for (ssize i = 0; i < view.entity_count; i++) {
            pos[i].x = i;
        }
    }
    {
        spargel::ecs::component_id ids[] = {position_id, velocity_id};
        struct spargel::ecs::spawn_descriptor desc = {
            .component_count = 2,
            .components = ids,
            .entity_count = 20,
        };
        spargel::ecs::spawn_entities(world, &desc, &view);
        printf("info: spawned 20 x [ position, velocity ] with archetype %lu\n", view.archetype_id);
        struct position* pos = (struct position*)components[0];
        struct velocity* vel = (struct velocity*)components[1];
        for (ssize i = 0; i < view.entity_count; i++) {
            pos[i].x = -i;
            vel[i].v = i;
        }
    }
    {
        spargel::ecs::component_id ids[] = {position_id, health_id, velocity_id};
        struct spargel::ecs::spawn_descriptor desc = {
            .component_count = 3,
            .components = ids,
            .entity_count = 50,
        };
        spargel::ecs::spawn_entities(world, &desc, &view);
        printf(
            "info: spawned 50 x [ position, health, velocity ] with archetype "
            "%lu\n",
            view.archetype_id);
        struct position* pos = (struct position*)components[0];
        struct health* h = (struct health*)components[1];
        struct velocity* vel = (struct velocity*)components[2];
        for (ssize i = 0; i < view.entity_count; i++) {
            pos[i].x = -i;
            h[i].h = 100;
            vel[i].v = i;
        }
    }
    {
        spargel::ecs::component_id ids[] = {velocity_id};
        struct spargel::ecs::spawn_descriptor desc = {
            .component_count = 1,
            .components = ids,
            .entity_count = 5,
        };
        spargel::ecs::spawn_entities(world, &desc, &view);
        printf("info: spawned 5 x [ velocity ] with archetype %lu\n", view.archetype_id);
    }

    {
        spargel::ecs::component_id ids[] = {position_id};
        u64 archetype_id = 0;
        struct spargel::ecs::query_descriptor desc = {
            .component_count = 1,
            .components = ids,
        };
        while (true) {
            desc.start_archetype_id = archetype_id;
            int result = spargel::ecs::query(world, &desc, &view);
            if (result == spargel::ecs::RESULT_QUERY_END) break;
            printf("info: query: get %ld entities with [ position ] in archetype %lu\n",
                   view.entity_count, view.archetype_id);
            archetype_id = view.archetype_id + 1;

            struct position* pos = (struct position*)components[0];
            for (ssize i = 0; i < view.entity_count; i++) {
                printf("info: query:  - entity %lu [ position = %.2f ]\n", view.entities[i],
                       pos[i].x);
            }
        }
    }

    {
        spargel::ecs::component_id ids[] = {health_id};
        u64 archetype_id = 0;
        struct spargel::ecs::query_descriptor desc = {
            .component_count = 1,
            .components = ids,
        };
        while (true) {
            desc.start_archetype_id = archetype_id;
            int result = spargel::ecs::query(world, &desc, &view);
            if (result == spargel::ecs::RESULT_QUERY_END) break;
            printf("info: query: get %ld entities with [ health ] in archetype %lu\n",
                   view.entity_count, view.archetype_id);
            archetype_id = view.archetype_id + 1;

            struct health* h = (struct health*)components[0];
            for (ssize i = 0; i < view.entity_count; i++) {
                printf("info: query:  - entity %lu [ health = %.2f ]\n", view.entities[i], h[i].h);
            }
        }
    }

    {
        spargel::ecs::component_id ids[] = {position_id, velocity_id};
        struct spargel::ecs::spawn_descriptor desc = {
            .component_count = 2,
            .components = ids,
            .entity_count = 2000,
        };
        spargel::ecs::spawn_entities(world, &desc, &view);
        printf("info: spawned 2000 x [ position, velocity ] with archetype %lu\n",
               view.archetype_id);
        struct position* pos = (struct position*)components[0];
        struct velocity* vel = (struct velocity*)components[1];
        for (ssize i = 0; i < view.entity_count; i++) {
            pos[i].x = -i;
            vel[i].v = i;
        }
    }

    struct delete_queue queue = {};

    {
        spargel::ecs::component_id ids[] = {position_id};
        u64 archetype_id = 0;
        struct spargel::ecs::query_descriptor desc = {
            .component_count = 1,
            .components = ids,
        };
        while (true) {
            desc.start_archetype_id = archetype_id;
            int result = spargel::ecs::query(world, &desc, &view);
            if (result == spargel::ecs::RESULT_QUERY_END) break;
            printf("info: query: get %ld entities with [ position ] in archetype %lu\n",
                   view.entity_count, view.archetype_id);
            archetype_id = view.archetype_id + 1;

            struct position* pos = (struct position*)components[0];
            for (ssize i = 0; i < view.entity_count; i++) {
                if (pos[i].x >= 5 || pos[i].x <= -5) {
                    delete_queue_push(&queue, view.entities[i]);
                }
            }
        }
    }

    spargel::ecs::delete_entities(world, queue.count, queue.ids);
    printf("info: deleted %ld entities\n", queue.count);

    {
        spargel::ecs::component_id ids[] = {position_id};
        u64 archetype_id = 0;
        struct spargel::ecs::query_descriptor desc = {
            .component_count = 1,
            .components = ids,
        };
        while (true) {
            desc.start_archetype_id = archetype_id;
            int result = spargel::ecs::query(world, &desc, &view);
            if (result == spargel::ecs::RESULT_QUERY_END) break;
            printf("info: query: get %ld entities with [ position ] in archetype %lu\n",
                   view.entity_count, view.archetype_id);
            archetype_id = view.archetype_id + 1;

            struct position* pos = (struct position*)components[0];
            for (ssize i = 0; i < view.entity_count; i++) {
                printf("info: query:  - entity %lu [ position = %.2f ]\n", view.entities[i],
                       pos[i].x);
            }
        }
    }

    destroy_delete_queue(&queue);
    spargel::ecs::destroy_world(world);

    sbase_report_allocation();
    sbase_check_leak();
    return 0;
}
