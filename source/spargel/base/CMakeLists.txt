spargel_add_library(base)
target_sources(base
  PRIVATE
    allocator.cc
    string.cc
    unimplemented.cc
)

if (SPARGEL_IS_LINUX)
  target_sources(base PRIVATE get_executable_path_linux.cc)

  spargel_add_executable(demo_get_executable_path_linux)
  target_sources(demo_get_executable_path_linux PRIVATE demo_get_executable_path_linux.cc)
  target_link_libraries(demo_get_executable_path_linux PRIVATE base)
endif ()

if (SPARGEL_IS_WINDOWS)
  target_sources(base PRIVATE get_executable_path_win.cc)

  spargel_add_executable(demo_get_executable_path_win)
  target_sources(demo_get_executable_path_win PRIVATE demo_get_executable_path_win.cc)
  target_link_libraries(demo_get_executable_path_win PRIVATE base)
endif ()

spargel_add_executable(test_base_vector)
target_sources(test_base_vector PRIVATE vector_test.cc)
target_link_libraries(test_base_vector PRIVATE base)

spargel_add_executable(test_base_string)
target_sources(test_base_string PRIVATE string_test.cc)
target_link_libraries(test_base_string PRIVATE base)
