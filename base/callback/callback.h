#ifndef BASE_CALLBACK_CALLBACK_H_
#define BASE_CALLBACK_CALLBACK_H_

#include "base/compiler/unreachable.h"

namespace base::callback {

template <typename>
class OnceCallback;

template <typename R, typename... Args>
class OnceCallback<R(Args...)> {
 public:
  using ReturnType = R;

  OnceCallback() = default;
  ~OnceCallback() = default;

  OnceCallback(OnceCallback const&) = delete;
  OnceCallback operator=(OnceCallback const&) = delete;
  OnceCallback(OnceCallback&&) = default;
  OnceCallback& operator=(OnceCallback&&) = default;

  R Run(Args&&... args) const& { UNREACHABLE(); }
};

using OnceClosure = OnceCallback<void()>;

}  // namespace base::callback

#endif
