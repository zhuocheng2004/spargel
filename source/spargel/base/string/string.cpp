#include <spargel/base/string/string.h>
#include <string.h>

namespace spargel::base {

string::string(char const* str) : _bytes(str, str + strlen(str)) {}

string::cstring string::c_str() const
{
    char* data = static_cast<char*>(default_allocator{}.allocate(length() + 1));
    memcpy(data, _bytes.data(), length());
    data[length()] = 0;
    return string::cstring(data, length());
}

}  // namespace spargel::base
