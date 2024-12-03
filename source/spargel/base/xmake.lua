target("base")
    set_kind("static")
    add_files(
        "base_c.cpp",
        "allocator/allocator.cpp",
        "assert/assert.cpp",
        "memory/memory.cpp",
        "string/string.cpp",
        "test/test.cpp"
    )
    add_files(
        "base.mpp",
        "base_c.mpp",
        "algorithm/algorithm.mpp",
        "allocator/allocator.mpp",
        "assert/assert.mpp",
        "container/container.mpp",
        "coroutine/coroutine.mpp",
        "memory/memory.mpp",
        "meta/meta.mpp",
        "pointer/pointer.mpp",
        "string/string.mpp",
        "test/test.mpp",
        { public = true }
    )
    if is_plat("macosx") then
        add_defines("SPARGEL_IS_MACOS")
    end
    if is_plat("linux") then
        add_defines("SPARGEL_IS_LINUX")
    end
    if is_plat("windows") then
        add_defines("SPARGEL_IS_WINDOWS")
    end

target("base_test_main")
    set_kind("static")
    add_files("test/test_main.cpp")
    add_deps("base")

target("demo_get_executable_path")
    set_kind("binary")
    add_files("demo_get_executable_path.cpp")
    add_deps("base")

target("demo_backtrace")
    set_kind("binary")
    add_files("demo_backtrace.cpp")
    add_deps("base")

target("vector_test")
    set_kind("binary")
    add_files("container/vector_test.cpp")
    add_deps("base", "base_test_main")

target("string_test")
    set_kind("binary")
    add_files("string/string_test.cpp")
    add_deps("base", "base_test_main")

target("coroutine_test")
    set_kind("binary")
    add_files("coroutine/coroutine_test.cpp")
    add_deps("base", "base_test_main")

target("panic_demo")
    set_kind("binary")
    add_files("assert/panic_test.cpp")
    add_deps("base")

target("unreachable_demo")
    set_kind("binary")
    add_files("assert/unreachable_test.cpp")
    add_deps("base")
