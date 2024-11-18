spargel_add_executable(squares)
target_sources(squares PRIVATE main.cc ecs.cc)
target_link_libraries(squares PRIVATE glfw nanovg glad nanovg_perf Tracy::TracyClient)
spargel_target_add_resources_dir(squares)
