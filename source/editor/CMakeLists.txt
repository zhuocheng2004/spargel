spargel_add_executable(editor)
target_sources(editor PRIVATE main.cpp)
target_link_libraries(editor PRIVATE codec gpu renderer ui)

if(SPARGEL_IS_MACOS)
  add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/SpargelEditor.app/Contents/Info.plist
    COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist ${CMAKE_BINARY_DIR}/SpargelEditor.app/Contents/Info.plist
  )
  add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/SpargelEditor.app/Contents/MacOS/editor
    COMMAND cp ${CMAKE_BINARY_DIR}/source/editor/editor ${CMAKE_BINARY_DIR}/SpargelEditor.app/Contents/MacOS/editor
    DEPENDS editor
  )
  add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/SpargelEditor.app/Contents/Resources/shader.metallib
    COMMAND cp ${CMAKE_BINARY_DIR}/source/spargel/renderer/shader.metallib ${CMAKE_BINARY_DIR}/SpargelEditor.app/Contents/Resources/shader.metallib
    DEPENDS ${CMAKE_BINARY_DIR}/source/spargel/renderer/shader.metallib
  )
  add_custom_target(SpargelEditorBundle ALL
    DEPENDS
      ${CMAKE_BINARY_DIR}/SpargelEditor.app/Contents/Info.plist
      ${CMAKE_BINARY_DIR}/SpargelEditor.app/Contents/MacOS/editor
      # hack
      ${CMAKE_BINARY_DIR}/SpargelEditor.app/Contents/Resources/shader.metallib
  )
endif()
