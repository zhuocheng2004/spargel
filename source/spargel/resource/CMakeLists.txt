spargel_add_library(
    NAME resource
    PRIVATE
        resource.cpp
        empty.cpp
        directory.cpp
    DEPS
        base
)

spargel_add_executable(
    NAME demo_resource_empty
    PRIVATE demo_empty.cpp
    DEPS resource base
)

spargel_add_executable(
    NAME demo_resource_directory
    PRIVATE demo_directory.cpp
    DEPS resource base
)
