#include <spargel/base/allocator.h>

#include <stdlib.h>

#define SPARGEL_TRACE_ALLOCATION 1

#if SPARGEL_TRACE_ALLOCATION
#include <stdio.h>
#endif

namespace spargel::base {

void* DefaultAllocator::allocate(ssize size)
{
#if SPARGEL_TRACE_ALLOCATION
    printf("info: allocate size %lld\n", size);
#endif
    return malloc(size);
}

void DefaultAllocator::deallocate(void* ptr, ssize size)
{
#if SPARGEL_TRACE_ALLOCATION
    printf("info: free size %lld\n", size);
#endif
    free(ptr);
}

}  // namespace spargel::base
