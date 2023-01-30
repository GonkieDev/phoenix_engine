#include <core/engine.hpp>

#include <corecrt_malloc.h>
#include <stdlib.h>

void *
PXMemoryArenaAlloc(mem_arena *arena, s64 size)
{
    u8 *result = arena->head;

	if (result > arena->head + arena->size)
    {
        PX_ASSERT_MSG(0, "Ran out of memory in memory arena!");
        return 0;
    }

    arena->head = result;

    /* return (void*)result; */
    // TODO: fix this memory alignment thing 
    return malloc(size);
}
