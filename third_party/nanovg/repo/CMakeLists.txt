cmake_minimum_required(VERSION 3.20)

project(nanovg)

add_library(nanovg STATIC
  fontstash.h
	nanovg.c
	nanovg.h
	nanovg_gl.h
	nanovg_gl_utils.h
	stb_image.h
	stb_truetype.h
)
target_include_directories(nanovg PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

add_library(nanovg_perf STATIC
  nanovg_perf.h
  nanovg_perf.c
)
target_include_directories(nanovg_perf PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(nanovg_perf PUBLIC glad glfw nanovg)

# find_package(Freetype REQUIRED)
# target_compile_definitions(nanovg PUBLIC -DFONS_USE_FREETYPE)
# target_link_libraries(nanovg PUBLIC ${FREETYPE_LIBRARIES})
# target_include_directories(nanovg PUBLIC ${FREETYPE_INCLUDE_DIRS})
