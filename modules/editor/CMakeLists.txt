spargel_add_executable(editor)
target_sources(editor PRIVATE private/main.cc)
target_link_libraries(editor PRIVATE ui)
