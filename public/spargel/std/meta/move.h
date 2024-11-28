#pragma once

#include <spargel/std/meta/remove_reference.h>

namespace std {

template <typename T>
constexpr remove_reference<T>&& move(T&& t)
{
    return static_cast<remove_reference<T>&&>(t);
}

}  // namespace spargel::base
