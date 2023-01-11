#pragma once

#include "../defines.hpp"
#include "../platform/platform.hpp"
#include "memory_arenas.hpp"
#include "logger.hpp"
#include "asserts.h"

struct px_init_info
{
    u32 width = 720;
    u32 height = 480;
    wchar_t *gameName = L"Phoenix Engine Game";
};

b8 PXInit(px_init_info *initInfo);
