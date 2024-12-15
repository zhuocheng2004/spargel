
// libc
#include <cstdio>

// JNI & Android
#include <jni.h>

// Game Activity
#include <game-activity/native_app_glue/android_native_app_glue.h>

extern "C" {

#include "simple.h"

void android_main(struct android_app* pApp) {
    puts("Hello Android!");
    simple_entry(nullptr);
}
}
