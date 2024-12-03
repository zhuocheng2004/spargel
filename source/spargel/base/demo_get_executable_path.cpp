#include <stdio.h>

import spargel.base.c;

int main()
{
    struct sbase_string path = spgl_get_executable_path();
    printf("Current executable path: \"%s\" \n", path.data);
    printf("Path length: %td\n", path.length);
    sbase_string_deinit(path);
    return 0;
}
