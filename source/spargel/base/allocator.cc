#include <spargel/base/allocator.h>
#include <stdlib.h>

namespace spargel::base {

class GlobalAllocator final : public Allocator {
 public:
  GlobalAllocator();
  ~GlobalAllocator() override;

  u8* allocate(MemoryLayout layout) override;
  void deallocate(u8* ptr, MemoryLayout layout) override;
  u8* grow(u8* ptr, MemoryLayout old_layout, MemoryLayout new_layout) override;
};

GlobalAllocator::GlobalAllocator() {}
GlobalAllocator::~GlobalAllocator() {}

u8* GlobalAllocator::allocate(MemoryLayout layout) {
  return (u8*)malloc(layout.size);
}

void GlobalAllocator::deallocate(u8* ptr, MemoryLayout layout) { free(ptr); }

u8* GlobalAllocator::grow(u8* ptr, MemoryLayout old_layout,
                          MemoryLayout new_layout) {
  return (u8*)realloc(ptr, new_layout.size);
}

Allocator* Allocator::global() {
  static GlobalAllocator allocator;
  return &allocator;
}

Allocator::~Allocator() {}

}  // namespace spargel::base
