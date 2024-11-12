add_library(base STATIC)
target_compile_features(base PUBLIC cxx_std_20)
target_sources(base
  PUBLIC
    callback/callback.h
    compiler/has_builtin.h
    compiler/unreachable.h
    logging/logging.h
    memory/ref_count_ptr.h
    memory/ref_counted.h
    meta/forward.h
    meta/source_location.h
    task/task_runner.h
    task/sequenced_task_runner.h
    task/single_threaded_task_runner.h
  PRIVATE
    logging/logging.cc
    task/task_runner.cc
    task/sequenced_task_runner.cc
    task/single_threaded_task_runner.cc
)
target_include_directories(base PUBLIC "${PROJECT_SOURCE_DIR}")

add_executable(ref_counter_ptr_test memory/ref_count_ptr_test.cc)
target_compile_features(ref_counter_ptr_test PUBLIC cxx_std_20)
target_include_directories(ref_counter_ptr_test PUBLIC "${PROJECT_SOURCE_DIR}")
target_link_libraries(ref_counter_ptr_test base)
