
spargel_add_library(
    NAME entry
    PRIVATE
        simple.c
    PRIVATE_ANDROID
        simple_android.cpp
    DEPS
        base
)


if (SPARGEL_IS_ANDROID)
    target_link_libraries(entry PUBLIC
        android
        log
        game-activity::game-activity_static
    )

    add_library(simple_entry_demo SHARED simple_demo.c)
    target_link_libraries(simple_entry_demo PUBLIC entry)
    set_target_properties(simple_entry_demo PROPERTIES OUTPUT_NAME spargeldemo)
else ()
    spargel_add_executable(
        NAME simple_entry_demo
        PRIVATE simple_demo.c
        DEPS entry
    )
endif ()
