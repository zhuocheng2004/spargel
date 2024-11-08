find_library(FOUNDATION_LIBRARY Foundation)
find_library(APPKIT_LIBRARY AppKit)

if (APPLE)
  add_executable(mac_window mac_window.cc)
  target_compile_features(mac_window PUBLIC cxx_std_20)
  target_link_libraries(mac_window
    PRIVATE ${FOUNDATION_LIBRARY} ${APPKIT_LIBRARY})
endif ()

find_package(X11)
if (X11_FOUND AND X11_xcb_FOUND)
  add_executable(x11_window x11_window.c)
  target_include_directories(x11_window PRIVATE ${X11_INCLUDE_DIR} ${X11_xcb_INCLUDE_PATH})
  target_link_libraries(x11_window PRIVATE m ${X11_LIBRARIES} ${X11_xcb_LIB})
endif ()

add_executable(baby_ecs baby_ecs.cc)
target_compile_features(baby_ecs PUBLIC cxx_std_20)

add_executable(demo demo.cc ecs.cc)
target_compile_features(demo PUBLIC cxx_std_20)
target_link_libraries(demo PRIVATE glfw nanovg glad nanovg_perf Tracy::TracyClient)
