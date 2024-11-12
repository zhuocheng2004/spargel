#ifndef BASE_MEMORY_REF_COUNT_PTR_H_
#define BASE_MEMORY_REF_COUNT_PTR_H_

#include "base/meta/forward.h"

namespace base::memory {

template <typename T>
class RefCountPtr {
 public:
  ~RefCountPtr() {
    if (ptr_) {
      ptr_->Release();
    }
  }

  RefCountPtr(RefCountPtr const& that) {
    ptr_ = that.ptr_;
    if (ptr_) {
      ptr_->Retain();
    }
  }
  template <typename U>
  RefCountPtr(RefCountPtr<U> const& that) {
    ptr_ = that.ptr_;
    if (ptr_) {
      ptr_->Retain();
    }
  }
  RefCountPtr(RefCountPtr&& that) {
    ptr_ = that.ptr_;
    that.ptr_ = nullptr;
  }
  template <typename U>
  RefCountPtr(RefCountPtr<U>&& that) {
    ptr_ = that.ptr_;
    that.ptr_ = nullptr;
  }

  T* Get() const { return ptr_; }

 private:
  explicit RefCountPtr(T* ptr) : ptr_{ptr} {
    if (ptr_) {
      ptr_->Retain();
    }
  }
  template <typename U, typename... Args>
  friend RefCountPtr<U> MakeRefCounted(Args&&...);

  T* ptr_;
};

template <typename T, typename... Args>
RefCountPtr<T> MakeRefCounted(Args&&... args) {
  return RefCountPtr<T>(new T(meta::Forward<Args>(args)...));
}

}  // namespace base::memory

#endif
