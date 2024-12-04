spargel_add_library(
    NAME base
    PRIVATE
        base.c
        platform.c
    PRIVATE_LINUX
        platform_linux.c
    PRIVATE_MACOS
        platform_mac.c
    PRIVATE_WINDOWS
        platform_win.c
)

spargel_add_executable(
    NAME executable_path_demo
    PRIVATE executable_path_demo.c
    DEPS base
)

spargel_add_executable(
    NAME backtrace_demo
    PRIVATE backtrace_demo.c
    DEPS base
)

spargel_add_executable(
    NAME panic_demo
    PRIVATE panic_demo.c
    DEPS base
)
