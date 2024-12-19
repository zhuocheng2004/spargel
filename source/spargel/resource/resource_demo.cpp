
#include <spargel/base/base.h>
#include <spargel/base/logging.h>
#include <spargel/base/platform.h>
#include <spargel/entry/simple.h>
#include <spargel/resource/resource.h>
#include <spargel/util/path.h>

// libc
#include <stdio.h>

static void put_hex(u8 ch) {
    const char hex_digits[] = "0123456789abcdef";
    putchar(hex_digits[ch >> 4]);
    putchar(hex_digits[ch & 0xf]);
}

static void xxd_print(char* data, size_t size) {
    int last_line = (size + 1) / 16;
    for (int i = 0; i <= last_line; i++) {
        printf("%08x: ", 16 * i);
        int cnt = i == last_line ? size - 16 * i : 16;
        for (int j = 0; j < 16; j++) {
            if (j < cnt) {
                put_hex(data[16 * i + j]);
            } else {
                putchar(' ');
                putchar(' ');
            }
            if (j & 0x1) putchar(' ');
        }

        putchar(' ');

        for (int j = 0; j < cnt; j++) {
            char ch = data[16 * i + j];
            if (ch >= ' ' && ch <= '~')
                putchar(ch);
            else
                putchar('.');
        }

        putchar('\n');
    }
}

namespace spargel::entry {

    int simple_entry(simple_entry_data* entry_data) {
        base::string_view path = "abc.txt";

        auto resource = entry_data->resource_manager->open(resource::resource_id(path));
        if (!resource) {
            spargel_log_error("Cannot open resource \"%s\"", path.data());
            return 1;
        }

        size_t size = resource->size();
        spargel_log_info("Size of \"%s\": %ld", path.data(), size);
        spargel_log_info("================");

        char* data = (char*)resource->map_data();
        xxd_print(data, size);

        resource->close();
        delete resource;

        // Entry-provided resource manager will be closed automatically after returning.

        return 0;
    }

}  // namespace spargel::entry
