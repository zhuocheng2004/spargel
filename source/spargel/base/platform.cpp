#include <spargel/base/const.h>
#include <spargel/base/platform.h>

namespace spargel::base {

    string get_executable_path() {
        char* buf = (char*)allocate(PATH_MAX, ALLOCATION_BASE);
        usize len = _get_executable_path(buf, PATH_MAX);
        if (len >= PATH_MAX) {
            buf = (char*)reallocate(buf, PATH_MAX, len + 1, ALLOCATION_BASE);
            _get_executable_path(buf, len + 1);
        }
        buf[len] = '\0';
        string s;
        s._length = len;
        s._data = buf;
        return s;
    }

}  // namespace spargel::base
