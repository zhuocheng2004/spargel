add_library(base STATIC)
target_compile_features(base PUBLIC cxx_std_20)
target_sources(base
  PUBLIC
    callback/callback.h
    compiler/unreachable.h
    task/task_runner.h
    task/sequenced_task_runner.h
    task/single_threaded_task_runner.h
  PRIVATE
    task/task_runner.cc
    task/sequenced_task_runner.cc
    task/single_threaded_task_runner.cc
)
target_include_directories(base
  PUBLIC "${PROJECT_SOURCE_DIR}"
)
