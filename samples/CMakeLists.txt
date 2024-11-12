find_package(X11)
if (X11_FOUND AND X11_xcb_FOUND)
  add_executable(x11_window x11_window.c)
  target_include_directories(x11_window PRIVATE ${X11_INCLUDE_DIR} ${X11_xcb_INCLUDE_PATH})
  target_link_libraries(x11_window PRIVATE m ${X11_LIBRARIES} ${X11_xcb_LIB})
endif ()

add_executable(demo demo.cc ecs.cc)
target_compile_features(demo PUBLIC cxx_std_20)
target_link_libraries(demo PRIVATE glfw nanovg glad nanovg_perf Tracy::TracyClient)

add_executable(bit_array_demo bit_array_demo.cc)
target_compile_features(bit_array_demo PUBLIC cxx_std_20)
