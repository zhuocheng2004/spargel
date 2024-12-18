#include <spargel/base/base.h>
#include <spargel/base/string.h>

// libc
#include <string.h>

namespace spargel::base {

    string::string(string_view v) {
        _length = v.length();
        if (_length > 0) {
            _data = (char*)allocate(_length + 1, ALLOCATION_BASE);
            memcpy(_data, v.data(), _length);
            _data[_length] = 0;
        }
    }

    string::string(string const& other) {
        _length = other._length;
        if (_length > 0) {
            _data = (char*)allocate(_length + 1, ALLOCATION_BASE);
            memcpy(_data, other._data, _length);
            _data[_length] = 0;
        }
    }

    string& string::operator=(string const& other) {
        if (_length > 0) {
            deallocate(_data, _length + 1, ALLOCATION_BASE);
        }
        _length = other._length;
        if (_length > 0) {
            _data = (char*)allocate(_length + 1, ALLOCATION_BASE);
            memcpy(_data, other._data, _length);
            _data[_length] = 0;
        }
        return *this;
    }

    string::~string() {
        if (_data) deallocate(_data, _length + 1, ALLOCATION_BASE);
    }

    string string_from_cstr(char const* str) {
        usize len = strlen(str);
        return string_from_range(str, str + len);
    }

    string string_from_range(char const* begin, char const* end) {
        string str;
        str._length = end - begin;
        str._data = (char*)allocate(str._length + 1, ALLOCATION_BASE);
        memcpy(str._data, begin, str._length);
        str._data[str._length] = 0;
        return str;
    }

    bool operator==(string const& lhs, string const& rhs) {
        if (lhs._length != rhs._length) return false;
        return memcmp(lhs._data, rhs._data, lhs._length) == 0;
    }

    string string_concat(string const& str1, string const& str2) {
        string str;
        str._length = str1._length + str2._length;
        str._data = (char*)allocate(str._length + 1, ALLOCATION_BASE);
        memcpy(str._data, str1._data, str1._length);
        memcpy(str._data + str1._length, str2._data, str2._length);
        str._data[str._length] = '\0';
        return str;
    }

    string operator+(const string& lhs, const string& rhs) { return string_concat(lhs, rhs); }

    string operator+(const string& s, char ch) {
        string str;
        str._length = s._length + 1;
        str._data = (char*)allocate(str._length + 1, ALLOCATION_BASE);
        memcpy(str._data, s._data, s._length);
        str._data[s._length] = ch;
        str._data[s._length + 1] = '\0';
        return str;
    }

}  // namespace spargel::base
