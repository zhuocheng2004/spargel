spargel_add_library(
    NAME renderer
    PRIVATE_LINUX
        dummy.cpp
    PRIVATE_MACOS
        renderer_mtl.mm
    PRIVATE_WINDOWS
        dummy.cpp
)

if(SPARGEL_IS_MACOS)
  target_link_libraries(renderer PRIVATE ${FRAMEWORK_METAL} ${FRAMEWORK_QUARTZ_CORE})

  # compiler metal shaders
  # super hacky
  add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/shader.metallib
    COMMAND
      xcrun -sdk macosx metal -o ${CMAKE_CURRENT_BINARY_DIR}/shader.ir -c ${CMAKE_CURRENT_SOURCE_DIR}/shader.metal -I ${PROJECT_SOURCE_DIR}/source
    COMMAND
      xcrun -sdk macosx metallib -o ${CMAKE_CURRENT_BINARY_DIR}/shader.metallib ${CMAKE_CURRENT_BINARY_DIR}/shader.ir
    DEPENDS
      shader.metal
  )
  add_custom_target(renderer_metal_shader
    DEPENDS
      ${CMAKE_CURRENT_BINARY_DIR}/shader.metallib)
endif()
