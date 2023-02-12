# Phoenix Engine
Phoenix engine is a rendering engine. It is a thin layer of abstraction for Vulkan. It also provides platform independent input.

## FIXES:
- !!!! FIX PXMemoryArenaAllocate, needs to be aligned properly

## TODOs:

### Memory TODOs:
- memory logging
- init multiple arenas in same contiguous block
- Specific memory arena for renderer?
- memory allocate multiple structs at the same time - macro
- add casting to the allocation macro
- clear frame arena after initialisation and every frame

### Vulkan TODOs:
- move functions to vk helpers
- vulkan setup
- vulkan custom allocator
- device requirements should be driven by game / engine
- configurable vsync

### Renderer Backend TODOs:
- remove the arenas from function signatures since we have access to them inside the `vulkan_context`
- go through the things that can be reallocated and make sure they're being initalised to 0

#### Platform TODOs:
- minimise windows results in `engineState.suspended = 1`

#### Input TODOs:
- provide controller deadzone as parameters 
- settings file
- add more keys to win32 input switch statement
- mouse dx, dy
- FIX: sometimes inputs get processed twice?

#### Low priority TODOs:
- Get rid of VLAs that were used, not many but still

### Dependencies

### Flags
- `_DEBUG` to activate debug
- `#define PX_MAX_GAMEPADS [1-4]` before including the headers, this number should not be bigger than 4

### Reminders for myself

#### Tags to look for (for myself):
`xxmemoryxx`: memory related stuff
`xxoptimisationxx`: optimisation related stuff
