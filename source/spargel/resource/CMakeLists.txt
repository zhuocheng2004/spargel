spargel_add_library(resource)
target_sources(resource
  PRIVATE
    resource.cpp
    empty.cpp
    directory.cpp
)
target_link_libraries(resource PRIVATE base)

spargel_add_executable(demo_resource_empty)
target_sources(demo_resource_empty PRIVATE demo_empty.cpp)
target_link_libraries(demo_resource_empty PRIVATE resource base)

spargel_add_executable(demo_resource_directory)
target_sources(demo_resource_directory PRIVATE demo_directory.cpp)
target_link_libraries(demo_resource_directory PRIVATE resource base)
