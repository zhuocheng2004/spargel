
#include <spargel/base/logging.h>
#include <spargel/base/platform.h>
#include <spargel/entry/simple.h>

int simple_entry(spargel::entry::simple_entry_data& data) {
    spargel_log_info("running executable: %s", spargel::base::get_executable_path().data());

    //data.platform->start_loop();

    return 0;
}
