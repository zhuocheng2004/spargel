#pragma once

namespace spargel::base {

template <typename T>
constexpr void destroy_at(T* ptr)
{
    ptr->~T();
}

}  // namespace spargel::base
