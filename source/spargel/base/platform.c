#include <stdlib.h>

#include <spargel/base/const.h>
#include <spargel/base/platform.h>

struct sbase_string spgl_get_executable_path() {
    char* buf = (char*)malloc(PATH_MAX);
    ssize len = _spgl_get_executable_path(buf, PATH_MAX);
    if (len >= PATH_MAX) {
        buf = (char*)realloc(buf, len + 1);
        _spgl_get_executable_path(buf, len + 1);
    }
    buf[len] = '\0';
    return (struct sbase_string){
        .length = len,
        .data = buf,
    };
}
