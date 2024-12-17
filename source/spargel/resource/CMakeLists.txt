
spargel_add_library(
    NAME resource
    PRIVATE
        resource.cpp
        directory.cpp
    DEPS
        base
)

spargel_add_application(
    NAME resource_demo
    PRIVATE resource_demo.cpp
    DEPS
        resource
        util
        entry
)
spargel_application_add_resources(
    TARGET resource_demo
    SRC ${CMAKE_CURRENT_SOURCE_DIR}/demo_assets
    DST
    DEPS resource
)
