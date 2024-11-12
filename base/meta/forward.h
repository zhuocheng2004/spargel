#ifndef BASE_META_FORWARD_H_
#define BASE_META_FORWARD_H_

#include "base/compiler/has_builtin.h"

namespace base::meta {

namespace impl {

#if HAS_BUILTIN(__remove_reference_t)
template <typename T>
using RemoveReferenceType = __remove_reference_t(T);
#else
template <typename T>
struct RemoveReference {
  using Type = T;
};
template <typename T>
struct RemoveReference<T&> {
  using Type = T;
};
template <typename T>
struct RemoveReference<T&&> {
  using Type = T;
};
template <typename T>
using RemoveReferenceType = RemoveReference<T>::Type;
#endif

#if HAS_BUILTIN(__is_lvalue_reference)
template <typename T>
bool IsLValueReference = __is_lvalue_reference(T);
#else
template <typename T>
struct IsLValueReferenceHelper {
  static constexpr bool Value = false;
};
template <typename T>
struct IsLValueReferenceHelper<T&> {
  static constexpr bool Value = true;
};
template <typename T>
bool IsLValueReference = IsLValueReferenceHelper<T>::Value;
#endif

}  // namespace impl

template <typename T>
T&& Forward(impl::RemoveReferenceType<T>& value) {
  return static_cast<T&&>(value);
}

template <typename T>
T&& Forward(impl::RemoveReferenceType<T>&& value) {
  static_assert(false);
}

}  // namespace base::meta

#endif
