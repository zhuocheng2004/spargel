#pragma once

#include <string.h>

namespace spargel::base {

template <typename T>
struct DefaultCopyTraits {
  static void copyConstruct(T& dst, T const& src) {
    memcpy(&dst, &src, sizeof(T));
  }
};

template <typename T>
struct DefaultMoveTraits {
  static void moveConstruct(T& dst, T& src) { memcpy(&dst, &src, sizeof(T)); }
};

template <typename T, typename Traits = DefaultCopyTraits<T>>
void copyConstruct(T& dst, T const& src) {
  Traits::copyConstruct(dst, src);
}

template <typename T, typename Traits = DefaultMoveTraits<T>>
void moveConstruct(T& dst, T& src) {
  Traits::moveConstruct(dst, src);
}

}  // namespace spargel::base
