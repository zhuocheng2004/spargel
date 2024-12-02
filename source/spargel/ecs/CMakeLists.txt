spargel_add_library(ecs)
target_sources(ecs
  PRIVATE
    ecs.cpp
)
target_link_libraries(ecs PRIVATE base)

spargel_add_executable(ecs_test)
target_sources(ecs_test PRIVATE ecs_test.cpp)
target_link_libraries(ecs_test PRIVATE ecs)
