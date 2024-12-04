spargel_add_library(
    NAME codec
    PRIVATE
        cursor.c
        json.c
        ppm.c
    DEPS
        base
)

spargel_add_executable(
    NAME ppm_info
    PRIVATE ppm_info.c
    DEPS codec
)

spargel_add_executable(
    NAME dump_json
    PRIVATE dump_json.c
    DEPS codec
)
