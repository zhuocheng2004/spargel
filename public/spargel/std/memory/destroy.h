#pragma once

namespace std {

template <typename T>
constexpr void destroy_at(T* ptr)
{
    ptr->~T();
}

}  // namespace spargel::base
