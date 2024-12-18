spargel_add_library(
    NAME util
    PRIVATE
        path.cpp
    DEPS base
)

spargel_add_executable(
    NAME path_test
    PRIVATE
        path_test.cpp
    DEPS
        base
        util
)
