#include <spargel/base/string/string.h>
#include <string.h>

namespace spargel::base {

string::string(char const* str) : _bytes(str, str + strlen(str)) {}

cstring string::c_str() const
{
    char* data = static_cast<char*>(DefaultAllocator{}.allocate(length() + 1));
    memcpy(data, _bytes.data(), length());
    data[length()] = 0;
    return cstring(data, length());
}

}  // namespace spargel::base
