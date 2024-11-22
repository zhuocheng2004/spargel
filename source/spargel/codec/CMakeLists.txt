spargel_add_library(codec)
target_sources(codec
  PRIVATE
    codec.c
)

spargel_add_executable(ppm_info)
target_sources(ppm_info PRIVATE ppm_info.c)
target_link_libraries(ppm_info PRIVATE codec)
