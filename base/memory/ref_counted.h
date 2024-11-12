#ifndef BASE_MEMORY_REF_COUNTED_H_
#define BASE_MEMORY_REF_COUNTED_H_

#include <stdint.h>

namespace base::memory {

// Intrusive reference counting.
//
// Always access through RefCountPtr<T>
//
template <typename T>
class RefCounted {
 public:
  void Retain() { count_++; }
  void Release() {
    count_--;
    if (count_ == 0) {
      delete AsDerived();
    }
  }

 private:
  T* AsDerived() {
    return static_cast<T*>(this);
  }

  uint64_t count_;
};

}  // namespace base::memory

#endif
