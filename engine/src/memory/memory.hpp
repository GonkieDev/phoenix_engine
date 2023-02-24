#pragma once

#include <defines.hpp>

#if _DEBUG
typedef enum 
{
    MEMORY_ARENA_TYPE_GAME_PERM,
    MEMORY_ARENA_TYPE_GAME_FRAME,

    MEMORY_ARENA_TYPE_COUNT
} memory_arena_type_enum;
#endif

struct mem_arena
{
    u8  *buf;
    s64 offset;
    s64 size; // in bytes

#if _DEBUG
    memory_arena_type_enum arenaType;
#endif
};

#define PXMemoryArenaAllocStruct(arena, s) PXMemoryArenaAlloc(arena, sizeof(s))
#define PXMemoryArenaAllocStructs(arena, count, s) (s*)PXMemoryArenaAlloc(arena, sizeof(s)*count)

inline void *PXMemoryArenaAlloc(mem_arena *arena, s64 size, s64 align = 0 /*(sizeof(void*))*/);

inline void PXMemoryArenaClear(mem_arena *arena);
