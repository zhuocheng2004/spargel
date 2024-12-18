#include <spargel/base/allocator.h>
#include <spargel/base/assert.h>

// libc
#include <stdlib.h>

void* operator new(usize size, usize align) noexcept {
    return spargel::base::default_allocator()->alloc(size);
}

void* operator new[](usize size, usize align) noexcept {
    return spargel::base::default_allocator()->alloc(size);
}

void* operator new(usize count, void* ptr) noexcept { return ptr; }

void operator delete(void* ptr, usize size, usize align) noexcept {
    spargel::base::default_allocator()->free(ptr, size);
}

void operator delete[](void* ptr, usize size, usize align) noexcept {
    spargel::base::default_allocator()->free(ptr, size);
}

namespace spargel::base {

    allocator* default_allocator() { return libc_allocator::instance(); }

    libc_allocator* libc_allocator::instance() {
        static libc_allocator inst;
        return &inst;
    }

    void* libc_allocator::alloc(usize size) {
        spargel_assert(size > 0);
        return ::malloc(size);
    }

    void* libc_allocator::resize(void* ptr, usize old_size, usize new_size) {
        spargel_assert(ptr != nullptr);
        spargel_assert(old_size > 0);
        spargel_assert(new_size > 0);
        return ::realloc(ptr, new_size);
    }

    void libc_allocator::free(void* ptr, usize size) {
        spargel_assert(ptr != nullptr);
        spargel_assert(size > 0);
        ::free(ptr);
    }

}  // namespace spargel::base
