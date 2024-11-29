spargel_add_library(resource)
target_sources(resource
  PRIVATE
    resource.c
    empty.c
    directory.c
)

spargel_add_executable(demo_resource_empty)
target_sources(demo_resource_empty PRIVATE demo_empty.c)
target_link_libraries(demo_resource_empty PRIVATE resource base)

spargel_add_executable(demo_resource_directory)
target_sources(demo_resource_directory PRIVATE demo_directory.c)
target_link_libraries(demo_resource_directory PRIVATE resource base)
