#pragma once

#include <spargel/base/string_view.h>
#include <spargel/base/types.h>

namespace spargel::base {

    struct string {
        string() = default;

        string(string_view v);

        string(string const& other);
        string& operator=(string const& other);

        ~string();

        char& operator[](usize i) { return _data[i]; }
        char const& operator[](usize i) const { return _data[i]; }

        usize length() const { return _length; }
        char* begin() { return _data; }
        char const* begin() const { return _data; }
        char* end() { return _data + _length; }
        char const* end() const { return _data + _length; }
        char* data() { return _data; }
        char const* data() const { return _data; }
        string_view view() const { return string_view(_data, _data + _length); }

        friend bool operator==(string const& lhs, string const& rhs);

        friend string operator+(const string& lhs, const string& rhs);

        friend string operator+(const string& s, char ch);

        usize _length = 0;
        char* _data = nullptr;
    };

    string string_from_cstr(char const* str);

    string string_from_range(char const* begin, char const* end);

    string string_concat(string const& str1, string const& str2);

}  // namespace spargel::base
