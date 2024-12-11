#include <spargel/base/const.h>
#include <spargel/base/platform.h>

struct sbase_string spgl_get_executable_path() {
    char* buf = (char*)sbase_allocate(PATH_MAX, SBASE_ALLOCATION_BASE);
    ssize len = _spgl_get_executable_path(buf, PATH_MAX);
    if (len >= PATH_MAX) {
        buf = (char*)sbase_reallocate(buf, PATH_MAX, len + 1, SBASE_ALLOCATION_BASE);
        _spgl_get_executable_path(buf, len + 1);
    }
    buf[len] = '\0';
    return (struct sbase_string){
        .length = len,
        .data = buf,
    };
}
