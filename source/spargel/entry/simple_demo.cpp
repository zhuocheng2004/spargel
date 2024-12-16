
#include <spargel/base/logging.h>
#include <spargel/base/platform.h>
#include <stdio.h>

extern "C" {
int simple_entry(void* _todo);
}

int simple_entry(void* _todo) {
    spargel_log_info("running executable: %s", spargel::base::get_executable_path().data());

    return 0;
}
