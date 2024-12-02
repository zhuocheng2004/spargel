spargel_add_library(base)
target_sources(base
    PUBLIC FILE_SET CXX_MODULES FILES
        base.mpp
        base_c.mpp
        algorithm/algorithm.mpp
        allocator/allocator.mpp
        assert/assert.mpp
        container/container.mpp
        coroutine/coroutine.mpp
        memory/memory.mpp
        meta/meta.mpp
        string/string.mpp
        test/test.mpp
    PUBLIC
        base_c.cpp
        allocator/allocator.cpp
        assert/assert.cpp
        memory/memory.cpp
        string/string.cpp
        test/test.cpp
)

spargel_add_library(base_test_main)
target_sources(base_test_main
    PRIVATE
        test/test_main.cpp
)
target_link_libraries(base_test_main PRIVATE base)

if (SPARGEL_IS_LINUX)
    target_sources(base PRIVATE platform_linux.cpp)
endif ()

if (SPARGEL_IS_WINDOWS)
    target_sources(base PRIVATE platform_win.cpp)
endif ()

spargel_add_executable(demo_get_executable_path)
target_sources(demo_get_executable_path PRIVATE demo_get_executable_path.cpp)
target_link_libraries(demo_get_executable_path PRIVATE base)

spargel_add_executable(demo_backtrace)
target_sources(demo_backtrace PRIVATE demo_backtrace.cpp)
target_link_libraries(demo_backtrace PRIVATE base)

spargel_add_test(vector_test)
target_sources(vector_test PRIVATE container/vector_test.cpp)
target_link_libraries(vector_test PRIVATE base)

spargel_add_test(coroutine_test)
target_sources(coroutine_test PRIVATE coroutine/coroutine_test.cpp)
target_link_libraries(coroutine_test PRIVATE base base_test_main)

spargel_add_test(string_test)
target_sources(string_test PRIVATE string/string_test.cpp)
target_link_libraries(string_test PRIVATE base)

spargel_add_executable(panic_test)
target_sources(panic_test PRIVATE assert/panic_test.cpp)
target_link_libraries(panic_test PRIVATE base)

spargel_add_executable(unreachable_test)
target_sources(unreachable_test PRIVATE assert/unreachable_test.cpp)
target_link_libraries(unreachable_test PRIVATE base)
