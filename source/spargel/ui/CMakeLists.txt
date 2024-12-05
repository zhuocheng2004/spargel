spargel_add_library(
    NAME ui
    PRIVATE_LINUX
        ui_xcb.c
    PRIVATE_MACOS
        ui_mac.m
    PRIVATE_WINDOWS
        dummy.c
)

if (SPARGEL_IS_LINUX)
    find_package(X11)
    if (X11_FOUND AND X11_xcb_FOUND)
        target_include_directories(ui PRIVATE ${X11_INCLUDE_DIR} ${X11_xcb_INCLUDE_PATH})
        target_link_libraries(ui PRIVATE ${X11_LIBRARIES} ${X11_xcb_LIB})
    else ()
        message(FATAL_ERROR "X11/Xcb not found")
    endif ()
endif ()

if (APPLE)
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
