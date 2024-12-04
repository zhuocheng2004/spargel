#include <stdio.h>

#include <spargel/base/platform.h>

int main()
{
    struct sbase_string path = spgl_get_executable_path();
    printf("Current executable path: \"%s\" \n", path.data);
    printf("Path length: %lld\n", path.length);
    sbase_string_deinit(path);
    return 0;
}
