spargel_add_library(codec)
target_sources(codec
  PUBLIC FILE_SET CXX_MODULES FILES
    codec.mpp
  PRIVATE
    cursor.cpp
    codec.cpp
    #json.cpp
    #ppm.cpp
)
target_link_libraries(codec PRIVATE base)

spargel_add_executable(ppm_info)
target_sources(ppm_info PRIVATE ppm_info.cpp)
target_link_libraries(ppm_info PRIVATE codec)

spargel_add_executable(dump_json)
target_sources(dump_json PRIVATE dump_json.cpp)
target_link_libraries(dump_json PRIVATE codec)
