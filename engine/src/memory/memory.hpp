#pragma once

#include <defines.hpp>

struct mem_arena
{
    u8  *buf;
    s64 offset;
    s64 size; // in bytes
};
