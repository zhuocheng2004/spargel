spargel_add_library(resource)
target_sources(resource
  PRIVATE
    resource.c
    empty.c
)

spargel_add_executable(demo_resource_empty)
target_sources(demo_resource_empty PRIVATE demo_empty.c)
target_link_libraries(demo_resource_empty PRIVATE base resource)
