cmake_minimum_required(VERSION 3.28)

project(spargel)

add_subdirectory(base)
add_subdirectory(engine)
add_subdirectory(samples)

add_subdirectory(third_party/glad/repo)
add_subdirectory(third_party/glfw/repo)
add_subdirectory(third_party/nanovg/repo)

option(TRACY_ENALBE "" OFF)
add_subdirectory(third_party/tracy/repo)
