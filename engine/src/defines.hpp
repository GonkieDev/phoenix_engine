#pragma once

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

static_assert(sizeof(f32) == 4, "F32 must me 4 bytes!");


// PXAPI
#ifdef PXEXPORT

// Exports
#ifdef _MSC_VER
#define PXAPI __declspec(dllexport)
#else
#define PXAPI __attribute__((visibility("default")))
#endif // !_MSC_VER

#else // PXEXPORT

// Imports
#ifdef _MSC_VER
#define PXAPI __declspec(dllimport)
#else
#define PXAPI
#endif // !_MSC_VER

#endif // !PXEXPORT

#define KIBIBYTES(x) (x * 1024)
#define MEBIBYTES(x) (KIBIBYTES(x) * 1024)
#define GIBIBYTES(x) (MEBIBYTES(x) * 1024)

#define KILOBYTES(x) (x * 1000)
#define MEGABYTES(x) (KILOBYTES(x) * 1000)
#define GIGABYTES(x) (MEGABYTES(x) * 1000)


#define MIN(x, y) (x < y ? x : y)
#define MAX(x, y) (x > y ? x : y)
