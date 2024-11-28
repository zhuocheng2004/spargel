spargel_add_library(std)
target_sources(std
  PRIVATE
    allocator.cpp
)

spargel_add_executable(std_test)
target_sources(std_test PRIVATE std_test.cpp)
target_link_libraries(std_test PRIVATE std base)
