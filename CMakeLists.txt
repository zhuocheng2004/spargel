cmake_minimum_required(VERSION 3.28)

project(spargel)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(AddSpargel)

add_subdirectory(games)
add_subdirectory(modules)
add_subdirectory(samples)

add_subdirectory(third_party/glad/repo)
add_subdirectory(third_party/glfw/repo)
add_subdirectory(third_party/nanovg/repo)
add_subdirectory(third_party/tracy/repo)
add_subdirectory(third_party/vulkan_headers/repo)
