cmake_minimum_required(VERSION 3.28)

project(spargel)

add_subdirectory(engine)
add_subdirectory(samples)

add_subdirectory(third_party/glad)
add_subdirectory(third_party/glfw)
add_subdirectory(third_party/nanovg)

option(TRACY_ENALBE "" OFF)
add_subdirectory(third_party/tracy)
