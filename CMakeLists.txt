# we need to bump minimal cmake version to 3.29 to enable CMP0156
cmake_minimum_required(VERSION 3.28)

cmake_policy(SET CMP0156 NEW)

project(spargel)

set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(CTest)
include(AddSpargel)

#### System Detection ####

if(${CMAKE_SYSTEM_NAME} STREQUAL Android)
  set(SPARGEL_IS_ANDROID YES CACHE BOOL "")
else()
  set(SPARGEL_IS_ANDROID NO CACHE BOOL "")
endif()

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

#### UI ####

spargel_add_option(SPARGEL_UI_DUMMY "nogui" OFF)

#### GPU Backend ####

if(SPARGEL_IS_WINDOWS)
  set(SPARGEL_GPU_ENABLE_DIRECTX_DEFAULT ON)
else()
  set(SPARGEL_GPU_ENABLE_DIRECTX_DEFAULT OFF)
endif()

if(SPARGEL_IS_MACOS)
  set(SPARGEL_GPU_ENABLE_METAL_DEFAULT ON)
else()
  set(SPARGEL_GPU_ENABLE_METAL_DEFAULT OFF)
endif()

#### Spargel Options ####

spargel_add_option(SPARGEL_ENABLE_COVERAGE "enable coverge" OFF)
spargel_add_option(SPARGEL_ENABLE_ASSERT "enable assert()" ON)
spargel_add_option(SPARGEL_ENABLE_LOG_ANSI_COLOR "enable ANSI color for loggin" OFF)
spargel_add_option(SPARGEL_ENABLE_SANITIZER_ADDRESS "enable asan" ON)
spargel_add_option(SPARGEL_GPU_ENABLE_DIRECTX "enable directx backend" ${SPARGEL_GPU_ENABLE_DIRECTX_DEFAULT})
spargel_add_option(SPARGEL_GPU_ENABLE_METAL "enable metal backend" ${SPARGEL_GPU_ENABLE_METAL_DEFAULT})
spargel_add_option(SPARGEL_GPU_ENABLE_VULKAN "enable vulkan backend" ON)
spargel_add_option(SPARGEL_TRACE_ALLOCATION "trace allocation" ON)

#### External Libraries ####

if (SPARGEL_GPU_ENABLE_VULKAN)
    find_package(Vulkan REQUIRED glslangValidator)
endif ()

if (SPARGEL_IS_ANDROID)
  find_package(game-activity REQUIRED CONFIG)
endif ()

if (SPARGEL_IS_MACOS)
    find_library(FRAMEWORK_APP_KIT AppKit REQUIRED)
    find_library(FRAMEWORK_METAL Metal REQUIRED)
    find_library(FRAMEWORK_QUARTZ_CORE QuartzCore REQUIRED)
endif ()

add_subdirectory(games)
add_subdirectory(source)
