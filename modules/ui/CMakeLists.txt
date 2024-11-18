spargel_add_library(ui)
target_sources(ui
  PRIVATE
    private/platform.cc
    private/render_target.cc
    private/renderer.cc
    private/types.cc
    private/window.cc
    private/window_delegate.cc
)
if (APPLE)
  target_sources(ui
    PRIVATE
      private/appkit/platform_ns.mm
      private/appkit/window_ns.mm
      private/metal/render_target_mtl.mm
      private/metal/renderer_mtl.mm
  )

  find_library(FRAMEWORK_APP_KIT AppKit)
  find_library(FRAMEWORK_METAL Metal)
  find_library(FRAMEWORK_QUARTZ_CORE QuartzCore)
  target_link_libraries(ui ${FRAMEWORK_APP_KIT} ${FRAMEWORK_METAL} ${FRAMEWORK_QUARTZ_CORE})

  # compiler metal shaders
  # super hacky
  add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/shader/shader.metallib
    COMMAND
      xcrun -sdk macosx metal -o ${CMAKE_CURRENT_BINARY_DIR}/shader.ir -c ${CMAKE_CURRENT_SOURCE_DIR}/private/metal/shader.metal -I ${PROJECT_SOURCE_DIR}
    COMMAND
      xcrun -sdk macosx metallib -o ${CMAKE_BINARY_DIR}/shader/shader.metallib ${CMAKE_CURRENT_BINARY_DIR}/shader.ir
    DEPENDS
      private/metal/shader.metal
  )
  add_custom_target(ui_metal_shaderlib
    DEPENDS ${CMAKE_BINARY_DIR}/shader/shader.metallib)
endif ()
