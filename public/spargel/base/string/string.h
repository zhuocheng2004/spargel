#pragma once

#include <spargel/base/assert/assert.h>
#include <spargel/base/container/vector.h>
#include <spargel/base/string/cstring.h>

namespace spargel::base {

namespace __string {
class string {
public:
    string() = default;
    string(char const* str);

    /// @brief length of string, in bytes
    ssize length() const
    {
        return _bytes.count();
    }

    char& operator[](ssize i)
    {
        EXPECTS(i >= 0 && i < length());
        return _bytes[i];
    }
    char const& operator[](ssize i) const
    {
        EXPECTS(i >= 0 && i < length());
        return _bytes[i];
    }

    cstring c_str() const;

private:
    vector<char> _bytes;
};
}  // namespace __string

using __string::string;

}  // namespace spargel::base
