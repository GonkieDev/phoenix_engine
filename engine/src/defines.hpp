#pragma once

//
// Types
//

#include <stdint.h>

typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float  f32;
typedef double f64;

typedef i32 b32;
typedef i16 b16;
typedef i8  b8;

// typedef u32 s32; // Size in bytes
typedef u64 s64;

static_assert(sizeof(f32) == 4, "f32 must me 4 bytes!");
static_assert(sizeof(f64) == 8, "f64 must me 8 bytes!");


#define global_var static
#define local_persist static
#define internal static


//
// Platform detection
//
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)

// Windows stuff
#define PXPLATFORM_WINDOWS 1
#ifndef _WIN64
#error "64-bit is required on Windows!"
#endif

#else
#error "Platform not supported!"
#endif

#ifdef PXPLATFORM_WINDOWS

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#define VK_USE_PLATFORM_WIN32_KHR

// NOTE: this is so that linker can find wWinMain func
#pragma comment(linker, "/include:wWinMain")


//
// Engine flags
//

#define PXAPI internal

/* #define PXUSE_SIMD */

//
// Quick helpers
//

inline u32 PXDEBUGStrLen(char *buf)
{
    u32 result = 0;
    for (;;)
    {
        if (!buf[result])
            break;
        result++;
    }
    return result;
}

#define PXCLAMP(value, min, max) ( (value <= min) ? min : ((value >= max) ? max : value) )

#define KIBIBYTES(x) ((x) * 1024)
#define MEBIBYTES(x) (KIBIBYTES(x) * 1024)
#define GIBIBYTES(x) (MEBIBYTES( ((u64)x) ) * 1024)

#define KILOBYTES(x) ((x) * 1000)
#define MEGABYTES(x) (KILOBYTES(x) * 1000)
#define GIGABYTES(x) (MEGABYTES( ((u64)x) ) * 1000)

#define MILLISECONDS(t) ((t) * 1000)
#define MICROSECONDS(t) (MILLISECONDS(t) * 1000)
#define NANOSECONDS(t) (MILLISECONDS(t) * 1000)

#define ArrayLen(arr) (sizeof((arr))/sizeof((arr)[0]))

#define MIN(x, y) (x < y ? x : y)
#define MAX(x, y) (x > y ? x : y)


#endif // !PXPLATFORM_WINDOWS
