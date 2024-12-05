# we need to bump minimal cmake version to 3.29 to enable CMP0156
cmake_minimum_required(VERSION 3.28)

cmake_policy(SET CMP0156 NEW)

project(spargel)

set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(CTest)
include(AddSpargel)

if(${CMAKE_SYSTEM_NAME} STREQUAL Darwin)
  set(SPARGEL_IS_MACOS YES CACHE BOOL "")
else()
  set(SPARGEL_IS_MACOS NO CACHE BOOL "")
endif()

if(${CMAKE_SYSTEM_NAME} STREQUAL Linux)
  set(SPARGEL_IS_LINUX YES CACHE BOOL "")
else()
  set(SPARGEL_IS_LINUX NO CACHE BOOL "")
endif()

if(${CMAKE_SYSTEM_NAME} STREQUAL Windows)
  set(SPARGEL_IS_WINDOWS YES CACHE BOOL "")
else()
  set(SPARGEL_IS_WINDOWS NO CACHE BOOL "")
endif()

if(SPARGEL_IS_MACOS)
  find_library(FRAMEWORK_APP_KIT AppKit)
  find_library(FRAMEWORK_METAL Metal)
  find_library(FRAMEWORK_QUARTZ_CORE QuartzCore)
endif()

add_subdirectory(games)
add_subdirectory(source)

#add_subdirectory(third_party/glad/repo)
#add_subdirectory(third_party/glfw/repo)
#add_subdirectory(third_party/nanovg/repo)
#add_subdirectory(third_party/tracy/repo)
add_subdirectory(third_party/vulkan_headers/repo)
