#pragma once

#include "../defines.hpp"

struct mem_arena
{
    void *buf;
    u8   *head;
    s64  size; // in bytes
};

#define MemoryArenaAllocStruct(arena, s) MemoryArenaAlloc(arena, sizeof(s))

void *
MemoryArenaAlloc(mem_arena *arena, s32 size)
{
    assert(sizeof(void*) == 8);
    s32 alignedSize = size + sizeof(void*) - (size % sizeof(void*));
    u8 *result = arena->head + alignedSize;

	if (result > arena->head + arena->size)
    {
        assert(0);
        return 0;
    }

    arena->head = result;

    return (void*)result;
}
