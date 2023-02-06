#pragma once

#include <defines.hpp>

union v2
{
    f32 emts[2];
    struct
    {
        f32 x;
        f32 y;
    };
};


union v3
{
    f32 emts[3];
    struct
    {
        f32 x;
        f32 y;
        f32 z;
    };
};


union v4
{
#ifdef PXUSE_SIMD
    alignas(16) __m128 data;
#endif
    alignas(16) f32 emts[4];
    struct
    {
        f32 x;
        f32 y;
        f32 z;
        f32 w;
    };
};

typedef v4 quat;
