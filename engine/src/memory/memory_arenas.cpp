#include "../core/engine.hpp"

void *
MemoryArenaAlloc(mem_arena *arena, s64 size)
{
    u8 *result = arena->head;

	if (result > arena->head + arena->size)
    {
        return 0;
    }

    arena->head = result;

    return (void*)result;
}
