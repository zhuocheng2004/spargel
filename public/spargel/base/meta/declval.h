#pragma once

#include <spargel/base/meta/add_reference.h>

namespace spargel::base {

template <typename T>
add_rvalue_reference<T> declval()
{
    static_assert(false, "declval not allowed in an evaluated context");
}

}  // namespace spargel::base
