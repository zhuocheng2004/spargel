spargel_add_library(
    NAME gpu
    PRIVATE
        gpu.c
    DEPS
        base
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
      gpu_metal.m
  )
  target_link_libraries(gpu PRIVATE ${FRAMEWORK_METAL})
endif()

if(${SPARGEL_GPU_ENABLE_VULKAN})
  target_sources(gpu
    PRIVATE
      gpu_vulkan.c
  )
  target_link_libraries(gpu PRIVATE Vulkan::Headers)
endif()

spargel_add_executable(
    NAME demo_gpu
    PRIVATE demo_gpu.c
    DEPS gpu ui
)
