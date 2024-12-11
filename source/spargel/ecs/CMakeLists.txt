spargel_add_library(
    NAME ecs
    PRIVATE
        ecs.cpp
    DEPS
        base
)

spargel_add_executable(
    NAME ecs_demo
    PRIVATE ecs_demo.cpp
    DEPS ecs
)
