spargel_add_executable(
    NAME demo_gpu
    PRIVATE demo_gpu.cpp
    DEPS gpu ui
)
if (SPARGEL_IS_LINUX)
    target_link_libraries(demo_gpu PRIVATE m)
endif ()

if (SPARGEL_GPU_ENABLE_VULKAN)
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/vertex_shader.spirv
        COMMAND
            Vulkan::glslangValidator
                -S vert # stage
                --target-env vulkan1.2
                -o ${CMAKE_CURRENT_BINARY_DIR}/vertex_shader.spirv
                ${CMAKE_CURRENT_SOURCE_DIR}/vertex_shader.glsl
        DEPENDS vertex_shader.glsl
    )
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/vertex_shader.spirv.inc
        COMMAND
            xxd -i -n demo_gpu_vulkan_vertex_code ${CMAKE_CURRENT_BINARY_DIR}/vertex_shader.spirv ${CMAKE_CURRENT_BINARY_DIR}/vertex_shader.spirv.inc
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/vertex_shader.spirv
    )
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/fragment_shader.spirv
        COMMAND
            Vulkan::glslangValidator
                -S frag # stage
                --target-env vulkan1.2
                -o ${CMAKE_CURRENT_BINARY_DIR}/fragment_shader.spirv
                ${CMAKE_CURRENT_SOURCE_DIR}/fragment_shader.glsl
        DEPENDS fragment_shader.glsl
    )
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/fragment_shader.spirv.inc
        COMMAND
            xxd -i -n demo_gpu_vulkan_fragment_code ${CMAKE_CURRENT_BINARY_DIR}/fragment_shader.spirv ${CMAKE_CURRENT_BINARY_DIR}/fragment_shader.spirv.inc
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/fragment_shader.spirv
    )
    add_custom_target(demo_gpu_vulkan_shaders
        DEPENDS
            ${CMAKE_CURRENT_BINARY_DIR}/vertex_shader.spirv.inc
            ${CMAKE_CURRENT_BINARY_DIR}/fragment_shader.spirv.inc
    )
    add_dependencies(demo_gpu demo_gpu_vulkan_shaders)
endif ()

if (SPARGEL_GPU_ENABLE_METAL)
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/shader.ir
        COMMAND
            xcrun metal -o ${CMAKE_CURRENT_BINARY_DIR}/shader.ir -c ${CMAKE_CURRENT_SOURCE_DIR}/shader.metal
        DEPENDS shader.metal
    )
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/shader.metallib
        COMMAND
            xcrun metallib -o ${CMAKE_CURRENT_BINARY_DIR}/shader.metallib ${CMAKE_CURRENT_BINARY_DIR}/shader.ir
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/shader.ir
    )
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/shader.metallib.inc
        COMMAND
            xxd -i -n demo_gpu_metal_shader_code ${CMAKE_CURRENT_BINARY_DIR}/shader.metallib ${CMAKE_CURRENT_BINARY_DIR}/shader.metallib.inc
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/shader.metallib
    )
    add_custom_target(demo_gpu_metal_shaders
        DEPENDS
            ${CMAKE_CURRENT_BINARY_DIR}/shader.metallib.inc
    )
    add_dependencies(demo_gpu demo_gpu_metal_shaders)
endif ()

# spargel_add_executable(
#     NAME demo_gpu3
#     PRIVATE demo_gpu3.cpp
#     DEPS gpu ui
# )
