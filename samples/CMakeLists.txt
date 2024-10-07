find_library(FOUNDATION_LIBRARY Foundation)
find_library(APPKIT_LIBRARY AppKit)

add_executable(mac_window mac_window.cc)
target_compile_features(mac_window PUBLIC cxx_std_20)
target_link_libraries(mac_window
  PRIVATE ${FOUNDATION_LIBRARY} ${APPKIT_LIBRARY})
