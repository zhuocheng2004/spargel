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

spargel_add_option(SPARGEL_GPU_ENABLE_DIRECTX "enable directx backend" ${SPARGEL_GPU_ENABLE_DIRECTX_DEFAULT})
spargel_add_option(SPARGEL_GPU_ENABLE_METAL "enable metal backend" ${SPARGEL_GPU_ENABLE_METAL_DEFAULT})
spargel_add_option(SPARGEL_GPU_ENABLE_VULKAN "enable vulkan backend" ON)

spargel_add_library(gpu)
target_sources(gpu
  PRIVATE
    gpu.c
)
target_compile_definitions(gpu
  PRIVATE
    SPARGEL_GPU_ENABLE_DIRECTX=$<BOOL:${SPARGEL_GPU_ENABLE_DIRECTX}>
    SPARGEL_GPU_ENABLE_METAL=$<BOOL:${SPARGEL_GPU_ENABLE_METAL}>
    SPARGEL_GPU_ENABLE_VULKAN=$<BOOL:${SPARGEL_GPU_ENABLE_VULKAN}>
)
target_link_libraries(gpu PRIVATE base)

# if(${SPARGEL_GPU_ENABLE_DIRECTX})
#   target_sources(gpu
#     PRIVATE
#       gpu_dx.c
#   )
# endif()

if(${SPARGEL_GPU_ENABLE_METAL})
  target_sources(gpu
    PRIVATE
      gpu_mtl.m
  )
  target_link_libraries(gpu PRIVATE ${FRAMEWORK_METAL})
endif()

if(${SPARGEL_GPU_ENABLE_VULKAN})
  target_sources(gpu
    PRIVATE
      gpu_vk.c
  )
endif()
