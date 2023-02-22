// TOP
#if !defined(MEMORY_CPP)
#define MEMORY_CPP

#include <core/engine.hpp>

#include <corecrt_malloc.h>
#include <stdlib.h>

PXAPI inline b8
__IsPtrPowerOf2(uintptr_t x)
{
	return (x & (x-1)) == 0;
}

uintptr_t
__AlignForward(uintptr_t ptr, s64 align)
{
	uintptr_t a = (uintptr_t)align;
	PX_ASSERT(__IsPtrPowerOf2(a));

	// Same as (ptr % a) but faster as 'a' is a power of two
	uintptr_t modulo = ptr & (a-1);

	if (modulo != 0)
    {
        // Align ptr if not aligned
		ptr += a - modulo;
	}

	return ptr;
}

#define PXMemoryArenaAllocStruct(arena, s) PXMemoryArenaAlloc(arena, sizeof(s))
#define PXMemoryArenaAllocStructs(arena, count, s) (s*)PXMemoryArenaAlloc(arena, sizeof(s)*count)

PXAPI inline void *
/* PXMemoryArenaAlloc(mem_arena *arena, s64 size, s64 align = (sizeof(void*)) ) */
PXMemoryArenaAlloc(mem_arena *arena, s64 size, s64 align = 0)
{
#if 0
	// Align 'offset' forward to the specified alignment
	uintptr_t currPtr = (uintptr_t)arena->buf + (uintptr_t)arena->offset;
	uintptr_t offset = __AlignForward(currPtr, align);
	offset -= (uintptr_t)arena->buf;
	
	// Check to see if the backing memory has space left
	if (offset + size > arena->size)
	    {
	        PX_ASSERT_MSG(0, "Memory arena ran out of memory!");
	        return 0;
	}
	
    void *result = arena->buf + offset;
    arena->offset = offset + size;

    return result;
#endif

    void *result = (void*)(arena->buf + arena->offset);
    arena->offset += size;
    return result;
}

inline void
PXMemoryArenaClear(mem_arena *arena)
{
    arena->offset = 0;
}

#endif // #if !defined(MEMORY_CPP)
// BOTTOM
