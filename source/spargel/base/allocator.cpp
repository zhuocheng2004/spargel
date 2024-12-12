#include <spargel/base/allocator.h>
#include <spargel/base/assert.h>

// libc
#include <stdlib.h>

void* operator new(size_t size, size_t align) noexcept {
    return spargel::base::default_allocator::instance()->alloc(size);
}

void* operator new[](size_t size, size_t align) noexcept {
    return spargel::base::default_allocator::instance()->alloc(size);
}

void* operator new(size_t count, void* ptr) noexcept { return ptr; }

void operator delete(void* ptr, size_t size, size_t align) noexcept {
    spargel::base::default_allocator::instance()->free(ptr, size);
}

void operator delete[](void* ptr, size_t size, size_t align) noexcept {
    spargel::base::default_allocator::instance()->free(ptr, size);
}

namespace spargel::base {

    default_allocator* default_allocator::instance() {
        static default_allocator inst;
        return &inst;
    }

    void* default_allocator::alloc(ssize size) {
        spargel_assert(size > 0);
        return ::malloc(size);
    }

    void* default_allocator::resize(void* ptr, ssize old_size, ssize new_size) {
        spargel_assert(ptr != nullptr);
        spargel_assert(old_size > 0);
        spargel_assert(new_size > 0);
        return ::realloc(ptr, new_size);
    }

    void default_allocator::free(void* ptr, ssize size) {
        spargel_assert(ptr != nullptr);
        spargel_assert(size > 0);
        ::free(ptr);
    }

}  // namespace spargel::base
