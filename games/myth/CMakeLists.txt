add_executable(myth
  main.cc
)
target_compile_features(myth PUBLIC cxx_std_20)
target_include_directories(myth
  PUBLIC
  "${PROJECT_SOURCE_DIR}"
)
