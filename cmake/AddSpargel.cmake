# for targets in SpargelEngine
function(spargel_target_common name)
    target_compile_features(${name} PUBLIC cxx_std_23)
    set_target_properties(${name} PROPERTIES CXX_EXTENSIONS OFF)

    target_include_directories(${name}
        PUBLIC
            "${PROJECT_SOURCE_DIR}/source"
            "${CMAKE_BINARY_DIR}/source"
    )

  # set_target_properties(${name} PROPERTIES LINK_LIBRARIES_STRATEGY REORDER_FREELY)
    target_compile_options(${name}
        PRIVATE
            $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:
                -Wall
                -fmacro-prefix-map=${PROJECT_SOURCE_DIR}=
            >
    )
    if(${SPARGEL_ENABLE_SANITIZER_ADDRESS})
        target_compile_options(${name} PRIVATE $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-fsanitize=address>)
    endif()
    if(${SPARGEL_ENABLE_COVERAGE})
        target_compile_options(${name} PRIVATE $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:--coverage -fprofile-instr-generate -fcoverage-mapping>)
    endif()
endfunction()

function(spargel_add_executable)
    cmake_parse_arguments(ARGS
        "" # options
        "NAME" # one value
        "PRIVATE;DEPS;PRIVATE_ANDROID;PRIVATE_LINUX;PRIVATE_MACOS;PRIVATE_WINDOWS" # multi value
        ${ARGN})

    add_executable(${ARGS_NAME})
    spargel_target_common(${ARGS_NAME})
    target_sources(${ARGS_NAME} PRIVATE ${ARGS_PRIVATE})
    if (SPARGEL_IS_ANDROID)
        target_sources(${ARGS_NAME} PRIVATE ${ARGS_PRIVATE_ANDROID})
    endif ()
    if (SPARGEL_IS_LINUX)
        target_sources(${ARGS_NAME} PRIVATE ${ARGS_PRIVATE_LINUX})
    endif ()
    if (SPARGEL_IS_MACOS)
        target_sources(${ARGS_NAME} PRIVATE ${ARGS_PRIVATE_MACOS})
    endif ()
    if (SPARGEL_IS_WINDOWS)
        target_sources(${ARGS_NAME} PRIVATE ${ARGS_PRIVATE_WINDOWS})
    endif ()
    target_link_libraries(${ARGS_NAME} PRIVATE ${ARGS_DEPS})

    if(${SPARGEL_ENABLE_SANITIZER_ADDRESS})
        target_link_options(${ARGS_NAME} PRIVATE $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-fsanitize=address>)
    endif()
    if(${SPARGEL_ENABLE_COVERAGE})
        target_link_options(${ARGS_NAME} PRIVATE $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:--coverage -fprofile-instr-generate>)
    endif()
endfunction()

function(spargel_add_library)
    cmake_parse_arguments(ARGS
        "" # options
        "NAME" # one value
        "PUBLIC;PRIVATE;DEPS;PRIVATE_ANDROID;PRIVATE_LINUX;PRIVATE_MACOS;PRIVATE_WINDOWS" # multi value
        ${ARGN})
    add_library(${ARGS_NAME})
    spargel_target_common(${ARGS_NAME})
    target_sources(${ARGS_NAME} PUBLIC ${ARGS_PUBLIC} PRIVATE ${ARGS_PRIVATE})
    if (SPARGEL_IS_ANDROID)
        target_sources(${ARGS_NAME} PRIVATE ${ARGS_PRIVATE_ANDROID})
    endif ()
    if (SPARGEL_IS_LINUX)
        target_sources(${ARGS_NAME} PRIVATE ${ARGS_PRIVATE_LINUX})
    endif ()
    if (SPARGEL_IS_MACOS)
        target_sources(${ARGS_NAME} PRIVATE ${ARGS_PRIVATE_MACOS})
    endif ()
    if (SPARGEL_IS_WINDOWS)
        target_sources(${ARGS_NAME} PRIVATE ${ARGS_PRIVATE_WINDOWS})
    endif ()
    target_link_libraries(${ARGS_NAME} PRIVATE ${ARGS_DEPS})
endfunction()

function(spargel_add_test name)
    spargel_add_executable(NAME ${name})
    add_test(NAME ${name} COMMAND ${name})
    target_link_libraries(${name} PRIVATE base base_test_main)
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
