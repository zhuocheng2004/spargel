#pragma once

#include <spargel/base/meta/add_const.h>
#include <spargel/base/meta/add_reference.h>

namespace spargel::base {

template <typename L, typename R>
concept is_assignable = __is_assignable(L, R);

template <typename T>
concept is_copy_assignable =
    is_assignable<add_lvalue_reference<T>, add_lvalue_reference<add_const<T>>>;

template <typename T>
concept is_move_assignable =
    is_assignable<add_lvalue_reference<T>, add_rvalue_reference<T>>;

}  // namespace spargel::base