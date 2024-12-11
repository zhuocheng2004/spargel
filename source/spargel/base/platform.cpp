#include <spargel/base/const.h>
#include <spargel/base/platform.h>

namespace spargel::base {

    string get_executable_path() {
        char* buf = (char*)allocate(PATH_MAX, ALLOCATION_BASE);
        ssize len = _get_executable_path(buf, PATH_MAX);
        if (len >= PATH_MAX) {
            buf = (char*)reallocate(buf, PATH_MAX, len + 1, ALLOCATION_BASE);
            _get_executable_path(buf, len + 1);
        }
        buf[len] = '\0';
        return {
            .length = len,
            .data = buf,
        };
    }

}  // namespace spargel::base
