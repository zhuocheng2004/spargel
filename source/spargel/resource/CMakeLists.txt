spargel_add_library(
    NAME resource
    PRIVATE
        resource.c
        empty.c
        directory.c
    DEPS
        base
)

spargel_add_executable(
    NAME demo_resource_empty
    PRIVATE demo_empty.c
    DEPS resource base
)

spargel_add_executable(
    NAME demo_resource_directory
    PRIVATE demo_directory.c
    DEPS resource base
)
