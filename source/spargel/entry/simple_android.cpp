
#include <spargel/entry/simple.h>

// JNI & Android
#include <jni.h>

// Game Activity
#include <game-activity/native_app_glue/android_native_app_glue.h>

extern "C" {
void android_main(struct android_app* app);
}

void android_main(struct android_app* app) {
    spargel::entry::simple_entry_data data;
    simple_entry(data);
}
