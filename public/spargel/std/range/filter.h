#pragma once

namespace std {

template <typename V>
class view_interface {};

template <typename V, typename P>
class filter_view : view_interface<filter_view<V, P>> {
public:
    constexpr filter_view() = default;
};

}  // namespace spargel::base
