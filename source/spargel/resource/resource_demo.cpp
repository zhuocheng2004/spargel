
#include <spargel/base/base.h>
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

int simple_entry(spargel::entry::simple_entry_data* data) {
    spargel::resource::resource_manager* resource_manager =
        spargel::resource::default_resource_manager();

    spargel::base::string path = spargel::base::string_from_cstr("abc.txt");

    spargel::resource::resource* resource =
        resource_manager->open(spargel::resource::resource_id(path));
    if (!resource) {
        fprintf(stderr, "Cannot open resource \"%s\"\n", path.data());
        return 1;
    }

    size_t size = resource->size();
    printf("Size of \"%s\": %ld\n", path.data(), size);
    puts("================");

    char* buf = new char[size];
    resource->get_data(buf);
    int last_line = (size + 1) / 16;
    for (int i = 0; i <= last_line; i++) {
        printf("%08x: ", 16 * i);
        int cnt = i == last_line ? size - 16 * i : 16;
        for (int j = 0; j < 16; j++) {
            if (j < cnt) {
                put_hex(buf[16 * i + j]);
            } else {
                putchar(' ');
                putchar(' ');
            }
            if (j & 0x1) putchar(' ');
        }

        putchar(' ');

        for (int j = 0; j < cnt; j++) {
            char ch = buf[16 * i + j];
            if (ch >= ' ' && ch <= '~')
                putchar(ch);
            else
                putchar('.');
        }

        putchar('\n');
    }
    delete[] buf;

    resource->close();
    delete resource;

    resource_manager->close();
    delete resource_manager;

    return 0;
}
