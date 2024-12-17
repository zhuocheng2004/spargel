add_subdirectory(base)
add_subdirectory(codec)
add_subdirectory(ecs)
add_subdirectory(entry)
add_subdirectory(gpu)
add_subdirectory(resource)
add_subdirectory(ui)
add_subdirectory(util)

configure_file(autoconfig.h.in autoconfig.h @ONLY)

add_library(spargel_config INTERFACE ${CMAKE_CURRENT_BINARY_DIR}/autoconfig.h)
