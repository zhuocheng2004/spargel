spargel_add_library(
    NAME codec
    PRIVATE
        codec.cpp
        cursor.cpp
        json.cpp
        ppm.cpp
    DEPS
        base
)

spargel_add_executable(
    NAME ppm_info
    PRIVATE ppm_info.cpp
    DEPS codec
)

spargel_add_executable(
    NAME dump_json
    PRIVATE dump_json.cpp
    DEPS codec
)
