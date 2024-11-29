#pragma once

#include <spargel/base/container/vector.h>

namespace spargel::base {

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
        return _bytes[i];
    }
    char const& operator[](ssize i) const
    {
        return _bytes[i];
    }

    struct cstring {
        cstring() = default;
        cstring(char* d, ssize l) : data{d}, length{l} {}
        cstring(cstring&& other) {
            data = other.data;
            length = other.length;
            other.data = nullptr;
            other.length = 0;
        }
        ~cstring()
        {
            if (data != nullptr) {
                default_allocator{}.deallocate(data, length + 1);
            }
        }
        operator char*()
        {
            return data;
        }
        char* data = nullptr;
        ssize length = 0;
    };

    /// @warning caller should free
    /// todo: change this behaviour
    cstring c_str() const;

private:
    vector<char> _bytes;
};

}  // namespace spargel::base
