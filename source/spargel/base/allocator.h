#pragma once

#include <spargel/base/types.h>

/* libc */
#include <stddef.h>

void* operator new(size_t size, size_t align) noexcept;
void* operator new[](size_t size, size_t align) noexcept;
void* operator new(size_t count, void* ptr) noexcept;
void operator delete(void* ptr, size_t size, size_t align) noexcept;
void operator delete[](void* ptr, size_t size, size_t align) noexcept;

namespace spargel::base {

    struct allocator {
        // requires: size > 0
        virtual void* alloc(ssize size) = 0;
        // requires: ptr != nullptr, old_size > 0, new_size > 0
        virtual void* resize(void* ptr, ssize old_size, ssize new_size) = 0;
        // requires: ptr != nullptr, size > 0
        virtual void free(void* ptr, ssize size) = 0;
    };

    struct default_allocator final : allocator {
        static default_allocator* instance();

        void* alloc(ssize size) override;
        void* resize(void* ptr, ssize old_size, ssize new_size) override;
        void free(void* ptr, ssize size) override;
    };

}  // namespace spargel::base
