#include <spargel/base/assert.h>
#include <spargel/base/base.h>
#include <spargel/base/compiler.h>
#include <spargel/base/logging.h>

/* libc */
#include <stdio.h>
#include <stdlib.h>

namespace spargel::base {

    /* there is no need to report allocation or check memory leaks if the program panics */
    void panic_at(char const* file, char const* func, ssize line) {
        /* todo: rewrite fprintf */
        fprintf(stderr, "======== PANIC [%s:%s:%ld] ========\n", file, func, line);
        print_backtrace();
#if defined(SPARGEL_IS_CLANG) || defined(SPARGEL_IS_GCC)
        __builtin_trap();
#elif defined(SPARGEL_IS_MSVC)
        __assume(false);
#else
#error unimplemented
#endif
    }

    void panic() { panic_at("<unknown>", "<unknown>", 0); }

    struct alloc_stat {
        ssize current;
        ssize total;
    };

    static struct alloc_stat alloc_stats[_ALLOCATION_COUNT] = {};
    static char const* alloc_names[_ALLOCATION_COUNT] = {
        "/spargel/base", "/spargel/codec", "/spargel/ecs", "/spargel/gpu", "/spargel/ui",
    };

    void* allocate(ssize size, int tag) {
        spargel_assert(tag >= 0 && tag < _ALLOCATION_COUNT);
        void* ptr = malloc(size);
        alloc_stats[tag].current += size;
        alloc_stats[tag].total += size;
#if SPARGEL_TRACE_ALLOCATION
        spargel_log_info("allocated %ld bytes with tag %s", size, alloc_names[tag]);
#endif
        return ptr;
    }

    void* reallocate(void* ptr, ssize old_size, ssize new_size, int tag) {
        spargel_assert(tag >= 0 && tag < _ALLOCATION_COUNT);
        void* new_ptr = realloc(ptr, new_size);
        alloc_stats[tag].current += new_size - old_size;
        if (new_size > old_size) {
            alloc_stats[tag].total += new_size - old_size;
        }
        spargel_assert(alloc_stats[tag].current >= 0);
#if SPARGEL_TRACE_ALLOCATION
        spargel_log_info("reallocated %ld -> %ld bytes with tag %s", old_size, new_size,
                         alloc_names[tag]);
#endif
        return new_ptr;
    }

    void deallocate(void* ptr, ssize size, int tag) {
        spargel_assert(tag >= 0 && tag < _ALLOCATION_COUNT);
        free(ptr);
        alloc_stats[tag].current -= size;
        spargel_assert(alloc_stats[tag].current >= 0);
#if SPARGEL_TRACE_ALLOCATION
        spargel_log_info("deallocated %ld bytes with tag %s", size, alloc_names[tag]);
#endif
    }

    void report_allocation() {
        fprintf(stderr, "======== Allocation Report ========\n");
        for (int i = 0; i < _ALLOCATION_COUNT; i++) {
            fprintf(stderr, "  %s [ current = %ld bytes, total = %ld bytes ]\n", alloc_names[i],
                    alloc_stats[i].current, alloc_stats[i].total);
        }
    }

    void check_leak() {
        for (int i = 0; i < _ALLOCATION_COUNT; i++) {
            if (alloc_stats[i].current != 0) {
                spargel_log_error("leak detected in %s", alloc_names[i]);
            }
        }
    }

}  // namespace spargel::base
