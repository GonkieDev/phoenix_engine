#pragma once

#include <defines.hpp>

struct mem_arena
{
    void *buf;
    u8   *head;
    s64  size; // in bytes
};

#define PXMemoryArenaAllocStruct(arena, s) PXMemoryArenaAlloc(arena, sizeof(s))

// TODO: alignment
void *PXMemoryArenaAlloc(mem_arena *arena, s64 size);
