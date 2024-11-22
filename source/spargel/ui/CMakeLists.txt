spargel_add_library(ui)
target_sources(ui
  PRIVATE
    ui.c
)
if (APPLE)
  target_sources(ui
    PRIVATE
      ui_mac.m
  )

  find_library(FRAMEWORK_APP_KIT AppKit)
  find_library(FRAMEWORK_METAL Metal)
  find_library(FRAMEWORK_QUARTZ_CORE QuartzCore)
  target_link_libraries(ui ${FRAMEWORK_APP_KIT} ${FRAMEWORK_METAL} ${FRAMEWORK_QUARTZ_CORE})

  # compiler metal shaders
  # super hacky
  #add_custom_command(
  #  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/shader.metallib
  #  COMMAND
  #    xcrun -sdk macosx metal -o ${CMAKE_CURRENT_BINARY_DIR}/shader.ir -c ${CMAKE_CURRENT_SOURCE_DI#R}/metal/shader.metal -I ${PROJECT_SOURCE_DIR}/source
  #  COMMAND
  #    xcrun -sdk macosx metallib -o ${CMAKE_CURRENT_BINARY_DIR}/shader.metallib ${CMAKE_CURRENT_BINARY_DI#R}/shader.ir
  #  DEPENDS
  #    metal/shader.metal
  #)
  #add_custom_target(ui_metal_shader
  #  DEPENDS
  #    ${CMAKE_CURRENT_BINARY_DIR}/shader.metallib)
endif ()
