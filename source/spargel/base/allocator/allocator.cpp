#include <spargel/base/memory/allocator.h>
#include <stdlib.h>

#define SPARGEL_TRACE_ALLOCATION 1

#if SPARGEL_TRACE_ALLOCATION
#include <stdio.h>
#endif

namespace spargel::base {

void* default_allocator::allocate(ssize size)
{
#if SPARGEL_TRACE_ALLOCATION
    printf("info: allocate size %td\n", size);
#endif
    return malloc(size);
}

void default_allocator::deallocate(void* ptr, ssize size)
{
#if SPARGEL_TRACE_ALLOCATION
    printf("info: free size %td\n", size);
#endif
    free(ptr);
}

}  // namespace spargel::base