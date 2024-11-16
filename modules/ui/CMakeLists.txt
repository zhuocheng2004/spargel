spargel_add_library(ui)
target_sources(ui
  PRIVATE
    private/platform.cc
    private/window.cc
)
if (APPLE)
  target_sources(ui
    PRIVATE
      private/appkit/platform_ns.mm
      private/appkit/window_ns.mm
  )

  find_library(APP_KIT AppKit)
  target_link_libraries(ui ${APP_KIT})
endif ()
