find_package(X11)
if (X11_FOUND AND X11_xcb_FOUND)
  add_executable(x11_window x11_window.c)
  target_include_directories(x11_window PRIVATE ${X11_INCLUDE_DIR} ${X11_xcb_INCLUDE_PATH})
  target_link_libraries(x11_window PRIVATE m ${X11_LIBRARIES} ${X11_xcb_LIB})
endif ()

spargel_add_executable(demo)
target_sources(demo PRIVATE demo.cc ecs.cc)
target_link_libraries(demo PRIVATE glfw nanovg glad nanovg_perf Tracy::TracyClient)

spargel_add_executable(bit_array_demo bit_array_demo.cc)
target_sources(bit_array_demo PRIVATE bit_array_demo.cc)

add_subdirectory(vulkan_triangle)
