spargel_add_library(renderer)

if(SPARGEL_IS_MACOS)
  target_sources(renderer
    PRIVATE
      renderer_mtl.mm
  )
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

# TODO
if (SPARGEL_IS_LINUX)
  target_sources(renderer PRIVATE dummy.cpp)
endif ()

# TODO
if (SPARGEL_IS_WINDOWS)
  target_sources(renderer PRIVATE dummy.cpp)
endif ()
