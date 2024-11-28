# for targets in SpargelEngine
function(spargel_target_common name)
  target_compile_features(${name} PUBLIC cxx_std_23)
  target_include_directories(${name}
    PUBLIC
      "${PROJECT_SOURCE_DIR}/public"
      "${PROJECT_SOURCE_DIR}/source"
  )
  # set_target_properties(${name} PROPERTIES LINK_LIBRARIES_STRATEGY REORDER_FREELY)
  target_compile_options(${name} PRIVATE $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall>)
  if(${SPARGEL_ENABLE_SANITIZER_ADDRESS})
    target_compile_options(${name} PRIVATE $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-fsanitize=address>)
  endif()
  if(${SPARGEL_ENABLE_COVERAGE})
    target_compile_options(${name} PRIVATE $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:--coverage -fprofile-instr-generate -fcoverage-mapping>)
  endif()
endfunction()

function(spargel_add_executable name)
  add_executable(${name})
  spargel_target_common(${name})
  if(${SPARGEL_ENABLE_SANITIZER_ADDRESS})
    target_link_options(${name} PRIVATE $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-fsanitize=address>)
  endif()
  if(${SPARGEL_ENABLE_COVERAGE})
    target_link_options(${name} PRIVATE $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:--coverage -fprofile-instr-generate>)
  endif()
endfunction()

function(spargel_add_library name)
  add_library(${name})
  spargel_target_common(${name})
endfunction()

function(spargel_add_option name doc value)
  option(${name} ${doc} ${value})
  if(${name})
    message(STATUS "${name}: ON")
  else()
    message(STATUS "${name}: OFF")
  endif()
endfunction()

function(spargel_target_add_resources_dir name)
  add_custom_target(${name}_copy_resources
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different ${CMAKE_CURRENT_SOURCE_DIR}/resources $<TARGET_PROPERTY:${name},BINARY_DIR>/resources
  )
  add_dependencies(${name} ${name}_copy_resources)
endfunction()
