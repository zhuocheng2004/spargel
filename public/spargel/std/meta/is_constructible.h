#pragma once

#include <spargel/std/meta/add_const.h>
#include <spargel/std/meta/add_reference.h>

namespace std {

template <typename T, typename... Args>
concept is_constructible = __is_constructible(T, Args...);

template <typename T>
concept is_copy_constructible =
    is_constructible<T, add_lvalue_reference<add_const<T>>>;

template <typename T>
concept is_move_constructible =
    is_constructible<T, add_rvalue_reference<T>>;

}  // namespace spargel::base
