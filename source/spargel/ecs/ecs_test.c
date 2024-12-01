#include <spargel/ecs/ecs.h>
#include <stdio.h>

struct position {
    float x;
};

struct velocity {
    float v;
};

struct health {
    float h;
};

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
        printf("info: spawned 10 x [ position ] with archetype id %llu\n",
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
            "info: spawned 20 x [ position, velocity ] with archetype id "
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
            "id "
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
        printf(
            "info: spawned 5 x [ velocity ] with archetype id "
            "%llu\n",
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

    secs_destroy_world(world);
    return 0;
}
