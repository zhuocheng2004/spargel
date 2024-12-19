spargel_add_library(
    NAME entry
    PRIVATE
        simple.cpp
    PRIVATE_ANDROID
        simple_android.cpp
    DEPS
        base
        resource
        ui
        util
)

spargel_add_executable(
    NAME launcher
    PRIVATE_MACOS
        launcher_mac.cpp
    DEPS
        base
        ui
)

spargel_add_component(
    NAME component_demo
    PRIVATE
        component_demo.cpp
    DEPS
        base
        ui
)

if (SPARGEL_IS_ANDROID)
    get_target_property(ANDROID_GAME_ACTIVITY_INCLUDE game-activity::game-activity INTERFACE_INCLUDE_DIRECTORIES)
    target_include_directories(entry PRIVATE ${ANDROID_GAME_ACTIVITY_INCLUDE})
    target_link_libraries(entry PRIVATE
        android
        log
        game-activity::game-activity_static
    )
endif ()

spargel_add_application(
    NAME simple_entry_demo
    PRIVATE
        simple_demo.cpp
    DEPS
        entry
)
