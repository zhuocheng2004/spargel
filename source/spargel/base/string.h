#pragma once

#include <spargel/base/base.h>

namespace spargel::base {

    /* string */

    struct string {
        string() = default;

        string(string const& other);
        string& operator=(string const& other);

        ~string();

        char& operator[](ssize i) { return _data[i]; }
        char const& operator[](ssize i) const { return _data[i]; }

        ssize length() const { return _length; }
        char* data() { return _data; }
        char const* data() const { return _data; }

        friend bool operator==(string const& lhs, string const& rhs);

        ssize _length = 0;
        char* _data = nullptr;
    };

    string string_from_cstr(char const* str);

    string string_from_range(char const* begin, char const* end);

    string string_concat(string const& str1, string const& str2);

}  // namespace spargel::base
