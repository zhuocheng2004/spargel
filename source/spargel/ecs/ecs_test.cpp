#include <stdio.h>
#include <stdlib.h>

#include <spargel/base/types.h>

import spargel.base.container;
import spargel.ecs;

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
    {
        secs_component_descriptor desc;

        desc.size = sizeof(position);
        secs_register_component(world, &desc, &position_id);

        desc.size = sizeof(velocity);
        secs_register_component(world, &desc, &velocity_id);

        desc.size = sizeof(health);
        secs_register_component(world, &desc, &health_id);
    }

    void* components[3];
    struct secs_view view = {.components = components};
    {
        secs_component_id ids[] = {position_id};
        secs_spawn_descriptor desc = {
            .component_count = 1,
            .components = ids,
            .entity_count = 10,
        };
        secs_spawn_entities(world, &desc, &view);
        printf("info: spawned 10 x [ position ] with archetype %llu\n",
               view.archetype_id);
        auto pos = (position*)components[0];
        for (ssize i = 0; i < view.entity_count; i++) {
            pos[i].x = i;
        }
    }
    {
        secs_component_id ids[] = {position_id, velocity_id};
        secs_spawn_descriptor desc = {
            .component_count = 2,
            .components = ids,
            .entity_count = 20,
        };
        secs_spawn_entities(world, &desc, &view);
        printf(
            "info: spawned 20 x [ position, velocity ] with archetype "
            "%llu\n",
            view.archetype_id);
        auto pos = (position*)components[0];
        auto vel = (velocity*)components[1];
        for (ssize i = 0; i < view.entity_count; i++) {
            pos[i].x = -i;
            vel[i].v = i;
        }
    }
    {
        secs_component_id ids[] = {position_id, health_id, velocity_id};
        secs_spawn_descriptor desc = {
            .component_count = 3,
            .components = ids,
            .entity_count = 50,
        };
        secs_spawn_entities(world, &desc, &view);
        printf(
            "info: spawned 50 x [ position, health, velocity ] with archetype "
            "%llu\n",
            view.archetype_id);
        auto pos = (position*)components[0];
        auto h = (health*)components[1];
        auto vel = (velocity*)components[2];
        for (ssize i = 0; i < view.entity_count; i++) {
            pos[i].x = -i;
            h[i].h = 100;
            vel[i].v = i;
        }
    }
    {
        secs_component_id ids[] = {velocity_id};
        secs_spawn_descriptor desc = {
            .component_count = 1,
            .components = ids,
            .entity_count = 5,
        };
        secs_spawn_entities(world, &desc, &view);
        printf("info: spawned 5 x [ velocity ] with archetype %llu\n",
               view.archetype_id);
    }

    {
        secs_component_id ids[] = {position_id};
        u64 archetype_id = 0;
        secs_query_descriptor desc = {
            .component_count = 1,
            .components = ids,
        };
        while (true) {
            desc.start_archetype_id = archetype_id;
            int result = secs_query(world, &desc, &view);
            if (result == SECS_RESULT_QUERY_END) break;
            printf(
                "info: query: get %td entities with [ position ] in archetype "
                "%llu\n",
                view.entity_count, view.archetype_id);
            archetype_id = view.archetype_id + 1;

            auto pos = (position*)components[0];
            for (ssize i = 0; i < view.entity_count; i++) {
                printf("info: query:  - entity %llu [ position = %.2f ]\n",
                       view.entities[i], pos[i].x);
            }
        }
    }

    {
        secs_component_id ids[] = {health_id};
        u64 archetype_id = 0;
        secs_query_descriptor desc = {
            .component_count = 1,
            .components = ids,
        };
        while (true) {
            desc.start_archetype_id = archetype_id;
            int result = secs_query(world, &desc, &view);
            if (result == SECS_RESULT_QUERY_END) break;
            printf(
                "info: query: get %td entities with [ health ] in archetype "
                "%llu\n",
                view.entity_count, view.archetype_id);
            archetype_id = view.archetype_id + 1;

            auto h = (health*)components[0];
            for (ssize i = 0; i < view.entity_count; i++) {
                printf("info: query:  - entity %llu [ health = %.2f ]\n",
                       view.entities[i], h[i].h);
            }
        }
    }

    {
        secs_component_id ids[] = {position_id, velocity_id};
        struct secs_spawn_descriptor desc = {
            .component_count = 2,
            .components = ids,
            .entity_count = 2000,
        };
        secs_spawn_entities(world, &desc, &view);
        printf(
            "info: spawned 2000 x [ position, velocity ] with archetype "
            "%llu\n",
            view.archetype_id);
        auto pos = (position*)components[0];
        auto vel = (velocity*)components[1];
        for (ssize i = 0; i < view.entity_count; i++) {
            pos[i].x = -i;
            vel[i].v = i;
        }
    }

    spargel::base::vector<secs_entity_id> queue;

    {
        secs_component_id ids[] = {position_id};
        u64 archetype_id = 0;
        secs_query_descriptor desc = {
            .component_count = 1,
            .components = ids,
        };
        while (true) {
            desc.start_archetype_id = archetype_id;
            int result = secs_query(world, &desc, &view);
            if (result == SECS_RESULT_QUERY_END) break;
            printf(
                "info: query: get %td entities with [ position ] in archetype "
                "%llu\n",
                view.entity_count, view.archetype_id);
            archetype_id = view.archetype_id + 1;

            auto pos = (position*)components[0];
            for (ssize i = 0; i < view.entity_count; i++) {
                if (pos[i].x >= 5 || pos[i].x <= -5) {
                    queue.push_back(view.entities[i]);
                }
            }
        }
    }

    secs_delete_entities(world, queue.count(), queue.data());
    printf("info: deleted %td entities\n", queue.count());

    {
        secs_component_id ids[] = {position_id};
        u64 archetype_id = 0;
        secs_query_descriptor desc = {
            .component_count = 1,
            .components = ids,
        };
        while (true) {
            desc.start_archetype_id = archetype_id;
            int result = secs_query(world, &desc, &view);
            if (result == SECS_RESULT_QUERY_END) break;
            printf(
                "info: query: get %td entities with [ position ] in archetype "
                "%llu\n",
                view.entity_count, view.archetype_id);
            archetype_id = view.archetype_id + 1;

            auto pos = (position*)components[0];
            for (ssize i = 0; i < view.entity_count; i++) {
                printf("info: query:  - entity %llu [ position = %.2f ]\n",
                       view.entities[i], pos[i].x);
            }
        }
    }

    secs_destroy_world(world);
    return 0;
}
