spargel_add_library(
    NAME gpu
    PRIVATE
        gpu.cpp
        gpu3.cpp
    DEPS
        base
        spargel_config
)

# if(${SPARGEL_GPU_ENABLE_DIRECTX})
#   target_sources(gpu
#     PRIVATE
#       gpu_dx.c
#   )
# endif()

if(${SPARGEL_GPU_ENABLE_METAL})
  target_sources(gpu
    PRIVATE
      gpu_metal.mm
      gpu3_metal.mm
  )
  target_link_libraries(gpu PRIVATE ${FRAMEWORK_METAL})
endif()

if(${SPARGEL_GPU_ENABLE_VULKAN})
  target_sources(gpu
    PRIVATE
      gpu_vulkan.cpp
  )
  target_link_libraries(gpu PRIVATE Vulkan::Headers)
endif()

add_subdirectory(demo)
