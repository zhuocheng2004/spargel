#pragma once

namespace std {

template <typename T>
concept is_destructible = __is_destructible(T);

template <typename T>
concept is_trivially_destructible = __is_trivially_destructible(T);

}  // namespace spargel::base
