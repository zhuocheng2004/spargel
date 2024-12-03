module;

#include <string.h>

module spargel.base.string;

namespace spargel::base {

string::string(char const* str) : _bytes(str, str + strlen(str)) {}

cstring string::c_str() const
{
    char* data = static_cast<char*>(default_allocator{}.allocate(length() + 1));
    memcpy(data, _bytes.data(), length());
    data[length()] = 0;
    return cstring(data, length());
}

}  // namespace spargel::base
