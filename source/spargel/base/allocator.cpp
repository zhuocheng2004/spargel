#include <spargel/base/memory/allocator.h>
#include <stdlib.h>

namespace spargel::base {

void* default_allocator::allocate(ssize size) { return malloc(size); }

void default_allocator::deallocate(void* ptr, ssize size) { free(ptr); }

}  // namespace spargel::base
