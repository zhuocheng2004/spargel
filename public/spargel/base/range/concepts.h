#pragma once

#include <spargel/base/meta/declval.h>

namespace spargel::base {

template <typename T>
concept range = requires(T& t) {
    begin(t);
    end(t);
};

template <range R>
using iterator_t = decltype(begin(declval<R&>()));

template <range R>
using sentinel_t = decltype(end(declval<R&>()));

}  // namespace spargel::base
