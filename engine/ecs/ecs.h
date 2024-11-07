/*
 * core definitions of the ECS architecture
 */

#ifndef SPARGEL_ENGINE_ECS_ECS_H
#define SPARGEL_ENGINE_ECS_ECS_H

#include <stddef.h>


typedef struct SpargelEcsWorld {
} SpargelEcsWorld;


typedef void (*SpargelSystemFunc)(SpargelEcsWorld* world);
typedef void (*SpargelUpdateFunc)(void* ptrs[], void* data);


void spargelEcsRegisterComponentType(SpargelEcsWorld* world, const char* name, size_t size);
void spargelEcsRegisterSystem(SpargelEcsWorld* world, const SpargelSystemFunc func);

void spargelAddGlobalComponent(SpargelEcsWorld* world, const char* name, size_t size);

void spargelEcsUpdate(SpargelEcsWorld* world);

void* spargelGetGlobalComponent(SpargelEcsWorld* world, const char* name);
void spargelEcsQueryAndUpdate(SpargelEcsWorld* world, const char* componentTypeNames[], const SpargelUpdateFunc func, void* data);


/* EXAMPLE */

static void exampleUpdate(void* ptrs[], void* data) {
    float timeDelta = *((float*) data);
    float *position = (float*) ptrs[0], *velocity = (float*) ptrs[1];
    *position += timeDelta * *velocity;
}

static void exampleSystem(SpargelEcsWorld* world) {
    float timeDelta = *((float*) spargelGetGlobalComponent(world, "time_delta"));

    const char* componentTypeNames[] = { "position", "velocity" }; 
    spargelEcsQueryAndUpdate(world, componentTypeNames, exampleUpdate, &timeDelta);
}

static void example() {
    SpargelEcsWorld world;

    spargelEcsRegisterComponentType(&world, "position", sizeof(float));
    spargelEcsRegisterComponentType(&world, "velocity", sizeof(float));

    spargelAddGlobalComponent(&world, "time_delta", sizeof(float));

    spargelEcsRegisterSystem(&world, exampleSystem);

    spargelEcsUpdate(&world);
}


#endif
