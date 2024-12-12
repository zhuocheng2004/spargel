#pragma once

#include <spargel/base/allocator.h>
#include <spargel/base/meta.h>

namespace spargel::base {

    template <typename T, typename... Args>
    void construct_at(T* ptr, Args&&... args) {
        new (ptr) T(forward<Args>(args)...);
    }

    template <typename T>
    void destruct_at(T* ptr) {
        ptr->~T();
    }

}  // namespace spargel::base
