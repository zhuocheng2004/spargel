#pragma once

#include <spargel/base/meta.h>

namespace spargel::base {

    template <typename T>
    struct vector {
        T* _begin = nullptr;
        T* _end = nullptr;
        T* _capacity = nullptr;
    };

}  // namespace spargel::base
