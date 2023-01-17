#pragma once

#include "../defines.hpp"

struct mem_arena
{
    void *buf;
    u8   *head;
    s64  size; // in bytes
};

#define MemoryArenaAllocStruct(arena, s) MemoryArenaAlloc(arena, sizeof(s))

void * MemoryArenaAlloc(mem_arena *arena, s64 size);
