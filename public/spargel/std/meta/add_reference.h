#pragma once

namespace std {

template <typename T>
using add_lvalue_reference = __add_lvalue_reference(T);

template <typename T>
using add_rvalue_reference = __add_rvalue_reference(T);

}  // namespace spargel::base
