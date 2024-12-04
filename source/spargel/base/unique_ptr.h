#pragma once

#include <spargel/base/memory/allocator.h>
#include <spargel/base/memory/destroy.h>

namespace spargel::base {

namespace __unique_ptr {

template <typename T>
class unique_ptr {
public:
    unique_ptr() = default;

    unique_ptr(T* ptr) : _ptr{ptr} {}

    unique_ptr(unique_ptr const&) = delete;
    unique_ptr(unique_ptr&& other) : _ptr{other._ptr}
    {
        other._ptr = nullptr;
    }

    ~unique_ptr()
    {
        if (_ptr != nullptr) {
            destroy_at(_ptr);
            default_allocator{}.deallocate(_ptr, sizeof(T));
        }
    }

    T* get()
    {
        return _ptr;
    }

    T* operator->()
    {
        return _ptr;
    }

private:
    T* _ptr = nullptr;
};

}  // namespace __unique_ptr

using __unique_ptr::unique_ptr;

}  // namespace spargel::base
