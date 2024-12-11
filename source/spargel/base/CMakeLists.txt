spargel_add_library(
    NAME base
    PRIVATE
        base.cpp
        platform.cpp
    PRIVATE_LINUX
        platform_linux.cpp
    PRIVATE_MACOS
        platform_mac.cpp
    PRIVATE_WINDOWS
        platform_win.cpp
    DEPS spargel_config
)

spargel_add_executable(
    NAME executable_path_demo
    PRIVATE executable_path_demo.cpp
    DEPS base
)

spargel_add_executable(
    NAME backtrace_demo
    PRIVATE backtrace_demo.cpp
    DEPS base
)

spargel_add_executable(
    NAME panic_demo
    PRIVATE panic_demo.cpp
    DEPS base
)

spargel_add_executable(
    NAME log_demo
    PRIVATE log_demo.cpp
    DEPS base
)
