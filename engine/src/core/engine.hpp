#pragma once

#include "../defines.hpp"
#include "../core/asserts.h"
#include "../core/logger.hpp"
#include "../core/memory_arenas.hpp"
#include "../core/engine.hpp"
#include "../platform/platform.hpp"

void GameMain(platform_state_ptr platformState);

b8 PhoenixInit(u32 width, u32 height, wchar_t *applicationName, platform_state_ptr platformState);
