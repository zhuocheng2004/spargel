#pragma once

namespace std {

// todo: allow custom compare function
template <typename T>
[[nodiscard]] constexpr T const& max(T const& a, T const& b)
{
    return a > b ? a : b;
}


}  // namespace spargel::base
