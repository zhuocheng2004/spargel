#pragma once

#include <spargel/std/meta/add_reference.h>

namespace std {

template <typename T>
add_rvalue_reference<T> declval()
{
    static_assert(false, "declval not allowed in an evaluated context");
}

}  // namespace spargel::base
