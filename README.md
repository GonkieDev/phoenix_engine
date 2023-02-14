# Phoenix Engine
![Phoenix Engine Logo](https://github.com/GonkieDev/phoenix_engine/blob/main/phoenixlogo.jpg)

Phoenix engine is a rendering engine. It is a thin layer of abstraction for Vulkan. It also provides platform independent input.

## FIXES:
- !!!! FIX PXMemoryArenaAllocate, needs to be aligned properly

## TODOs:

### Memory:
- memory logging
- init multiple arenas in same contiguous block
- Specific memory arena for renderer?
- memory allocate multiple structs at the same time - macro
- add casting to the allocation macro
- clear frame arena after initialisation and every frame

### Vulkan:
- move functions to vk helpers
- vulkan setup
- vulkan custom allocator
- device requirements should be driven by game / engine
- configurable vsync

### Renderer Backend:
- remove the arenas from function signatures since we have access to them inside the `vulkan_context`
- go through the things that can be reallocated and make sure they're being initalised to 0

### Platform:
- minimise windows results in `engineState.suspended = 1`

### Input:
- provide controller deadzone as parameters 
- settings file
- add more keys to win32 input switch statement
- mouse dx, dy
- FIX: sometimes inputs get processed twice?

### Filesystem:
- Use `mem_arena` for file buffer allocation?
- Platform agnostic file handle

### Logging:
- Log file

### Low priority TODOs:
- Get rid of VLAs that were used, not many but still
- Get rid of `PXAPI` (or at least change its name)

## Dependencies
- `stdb_image.h`

## Flags
- `_DEBUG` to activate debug
- `#define PX_MAX_GAMEPADS [1-4]` before including the headers, this number should not be bigger than 4

## Reminders for myself

### Tags to look for:
`xxmemoryxx`: memory related stuff
`xxoptimisationxx`: optimisation related stuff
`FIXME`
