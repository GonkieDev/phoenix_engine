# Phoenix Engine
![Phoenix Engine Logo](https://github.com/GonkieDev/phoenix_engine/blob/main/phoenixlogo.jpg)

Phoenix engine is a rendering engine. It is a thin layer of abstraction for Vulkan. It also provides platform independent input.

## FIXES:
- fix: allocating more memory every time screen is resized
- find out why does it crash when using default alignment size bigger than `2 * sizeof(void *)`

## TODOs:
- settings file

### Memory:
- Specific memory arena for renderer?
- write own `memset`

### Vulkan:
- move functions to vk helpers
- vulkan custom allocator
- device requirements should be driven by game / engine
- configurable vsync

### Renderer Backend:
- remove the arenas from function signatures since we have access to them inside the `vulkan_context`
- go through the things that can be reallocated and make sure they're being initalised to 0

### Platform:

### Input:
- provide controller deadzone as parameters 
- add more keys to win32 input switch statement
- mouse dx, dy
- FIX: sometimes inputs get processed twice?

### Filesystem:
- Use `mem_arena` for file buffer allocation?
- Platform agnostic file handle

### Logging:
- Should logging be debug mode only?

### Low priority TODOs:
- Get rid of VLAs that were used, not many but still
- Get rid of `PXAPI` (or at least change its name)
- Add guards to cpp files

## Dependencies

## Flags
- `_DEBUG` to activate debug
- `#define PX_MAX_GAMEPADS [1-4]` before including the headers, this number should not be bigger than 4

## Reminders for myself

### Tags to look for:
- `xxmemoryxx`: memory related stuff
- `xxoptimisationxx`: optimisation related stuff
- `FIXME`
- `PXRELEASE` : release build preprocessor macro
