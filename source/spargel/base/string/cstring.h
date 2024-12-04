#pragma once

#include <spargel/base/assert.h>
#include <spargel/base/allocator.h>

namespace spargel::base {

    class cstring {
    public:
        cstring() = default;

        cstring(char* data, ssize length) : _data{data}, _length{length} {}

        cstring(cstring&& other) {
            _data = other._data;
            _length = other._length;
            other._data = nullptr;
            other._length = 0;
        }

        ~cstring() {
            if (_data != nullptr) {
                DefaultAllocator{}.deallocate(_data, _length + 1);
            }
        }

        ssize length() const { return _length; }

        char* data() { return _data; }
        char const* data() const { return _data; }

        char& operator[](ssize i) {
            ASSERT(i >= 0 && i < length());
            return _data[i];
        }
        char const& operator[](ssize i) const {
            ASSERT(i >= 0 && i < length());
            return _data[i];
        }

    private:
        char* _data = nullptr;
        ssize _length = 0;
    };

}  // namespace spargel::base
