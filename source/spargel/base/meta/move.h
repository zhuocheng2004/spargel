#pragma once

#include <spargel/base/type_traits.h>

namespace spargel::base {

    template <typename T>
    constexpr remove_reference<T>&& move(T&& t) {
        return static_cast<remove_reference<T>&&>(t);
    }

}  // namespace spargel::base
