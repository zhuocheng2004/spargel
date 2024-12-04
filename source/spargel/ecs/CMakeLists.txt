spargel_add_library(
    NAME ecs
    PRIVATE
        ecs.c
    DEPS
        base
)

spargel_add_executable(
    NAME ecs_demo
    PRIVATE ecs_demo.c
    DEPS ecs
)
