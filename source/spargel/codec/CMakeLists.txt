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

spargel_add_executable(json_dump)
target_sources(json_dump PRIVATE json_dump.c)
target_link_libraries(json_dump PRIVATE codec)
