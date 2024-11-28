#pragma once

#include <spargel/std/meta/remove_reference.h>

namespace std {

template <typename T>
constexpr T&& forward(remove_reference<T>& t)
{
    return static_cast<T&&>(t);
}

}  // namespace spargel::base
