add_subdirectory(base)
add_subdirectory(codec)
add_subdirectory(ecs)
add_subdirectory(gpu)
add_subdirectory(resource)
add_subdirectory(ui)

configure_file(config.h.in config.h @ONLY)

add_library(spargel_config INTERFACE ${CMAKE_CURRENT_BINARY_DIR}/config.h)
