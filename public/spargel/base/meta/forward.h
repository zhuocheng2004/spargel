#pragma once

#include <spargel/base/meta/remove_reference.h>

namespace spargel::base {

template <typename T>
constexpr T&& forward(remove_reference<T>& t)
{
    return static_cast<T&&>(t);
}

}  // namespace spargel::base