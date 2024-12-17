
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
