#ifndef SPARGEL_ENGINE_ECS_ECS_H
#define SPARGEL_ENGINE_ECS_ECS_H

#include <stddef.h>
#include <functional>
#include <initializer_list>


namespace spargel::ecs {


class World;

class ISystem {
public:
    virtual void onCreate(World& world) { };

    virtual void onUpdate(World& world) = 0;
};

class World {
public:
    void registerComponentType(const char* name, size_t size);
    void registerSystem(ISystem& system);
    
    void addGlobalComponent(const char* name, size_t size);

    void update();

    void* getGlobalComponent(const char* name);
    void queryAndUpdate(const char* componentTypeNames[], std::function<void(void*[])> func);
    void queryAndUpdate(std::initializer_list<char*> componentTypeNames, std::function<void(void*[])> func);
};


/* EXAMPLE */

class ExampleSystem : public ISystem {
public:
    virtual void onUpdate(World& world) {
        float timeDelta = *static_cast<float*>(world.getGlobalComponent("time_delta"));

        world.queryAndUpdate({ "position", "velocity" }, [timeDelta](void* ptrs[]) -> void {
            float& position = *static_cast<float*>(ptrs[0]);
            float& velocity = *static_cast<float*>(ptrs[1]);
            position += timeDelta * velocity;
        });
    }
};

static void example() {
    World world;

    world.registerComponentType("position", sizeof(float));
    world.registerComponentType("velocity", sizeof(float));

    world.addGlobalComponent("time_delta", sizeof(float));

    ExampleSystem exampleSystem;
    world.registerSystem(exampleSystem);

    world.update();
}


}


#endif
