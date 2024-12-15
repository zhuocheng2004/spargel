
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

    # Mark the top-level symbol as used, otherwise the library will be optimized out.
    set(CMAKE_SHARED_LINKER_FLAGS
        "${CMAKE_SHARED_LINKER_FLAGS} -u \
        Java_com_google_androidgamesdk_GameActivity_initializeNativeCode")

    add_library(simple_entry_demo SHARED simple_demo.cpp)
    target_link_libraries(simple_entry_demo PUBLIC entry)
    set_target_properties(simple_entry_demo PROPERTIES OUTPUT_NAME spargeldemo)

else ()

    spargel_add_executable(
        NAME simple_entry_demo
        PRIVATE simple_demo.cpp
        DEPS entry
    )

endif ()
