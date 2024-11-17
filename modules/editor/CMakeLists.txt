spargel_add_executable(editor)
target_sources(editor PRIVATE private/main.cc)
target_link_libraries(editor PRIVATE ui)

if(SPARGEL_IS_MACOS)
  add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/SpargelEditor.app/Contents/Info.plist
    COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist ${CMAKE_BINARY_DIR}/SpargelEditor.app/Contents/Info.plist
  )
  add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/SpargelEditor.app/Contents/MacOS/editor
    COMMAND cp -Rc ${CMAKE_BINARY_DIR}/bin/editor ${CMAKE_BINARY_DIR}/SpargelEditor.app/Contents/MacOS/editor
  )
  add_custom_target(SpargelEditorBundle ALL
    DEPENDS
      ${CMAKE_BINARY_DIR}/SpargelEditor.app/Contents/Info.plist
      ${CMAKE_BINARY_DIR}/SpargelEditor.app/Contents/MacOS/editor
  )
endif()
