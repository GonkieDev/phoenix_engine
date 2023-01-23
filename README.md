# Phoenix Engine
Phoenix engine is a rendering engine. It is a thin layer of abstraction for Vulkan. It also provides platform independent input.

## TODOs:
- vulkan setup
- vulkan custom allocator
- memory logging
- init multiple arenas in same contiguous block

### Platform TODOs:
- minimise windows results in `engineState.suspended = 1`

### Input TODOs:
- provide controller deadzone as parameters 
- settings file
- add more keys to win32 input switch statement
- mouse dx, dy
- FIX: sometimes inputs get processed twice?

## Dependencies
- GLM

## Flags
- `_DEBUG` to activate debug
- `#define PX_MAX_GAMEPADS [1-4]` before including the headers, this number should not be bigger than 4
