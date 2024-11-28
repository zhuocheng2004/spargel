#include <spargel/std/memory/allocator.h>
#include <stdlib.h>

namespace std {

void* default_allocator::allocate(ssize size) { return malloc(size); }

void default_allocator::deallocate(void* ptr, ssize size) { free(ptr); }

}  // namespace spargel::base
