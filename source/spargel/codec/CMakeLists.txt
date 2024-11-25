spargel_add_library(codec)
target_sources(codec
  PRIVATE
    cursor.c
    json.c
    ppm.c
)
target_link_libraries(codec PRIVATE base)

spargel_add_executable(ppm_info)
target_sources(ppm_info PRIVATE ppm_info.c)
target_link_libraries(ppm_info PRIVATE codec)

spargel_add_executable(dump_json)
target_sources(dump_json PRIVATE dump_json.c)
target_link_libraries(dump_json PRIVATE codec)
