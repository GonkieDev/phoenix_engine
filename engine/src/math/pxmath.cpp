#include <math/pxmath.hpp>

// ---------------------------------------- 
//              General Math
// ---------------------------------------- 

// TODO
inline f32
PXSin(f32 x)
{
    return 0.f;
}

inline f32
PXCos(f32 x)
{
    return 0.f;
}

inline f32
PXTan(f32 x)
{
    return 0.f;
}

inline f32
PXASin(f32 x)
{
    return 0.f;
}

inline f32
PXACos(f32 x)
{
    return 0.f;
}

inline f32
PXSqrt(f32 x)
{
    return 0.f;
}

inline f32
PXAbs(f32 x)
{
    return 0.f;
}

// NOTE: I hate C++
template<typename T>
inline b8 IsPowerOf2(T x)
{
    static_assert(typeof(T) == typeof(i8)  || typeof(T) == typeof(u8)  ||
                  typeof(T) == typeof(i16) || typeof(T) == typeof(u16) ||
                  typeof(T) == typeof(i32) || typeof(T) == typeof(u32) ||
                  typeof(T) == typeof(i64) || typeof(T) == typeof(u64),
                  "Must be a number type.");

    return (value != 0) && (value & (value - 1)) == 0;
}

inline i32
PXRandomI32()
{}

inline i32
PXRandomI32InRange(i32 min, i32 max)
{}

// Returns random number between 0 and 1 (inclusive?)
inline f32
PXRandom()
{}

inline f32
PXRandomInRange(f32 min, f32 max)
{
    f32 result = min + ((max - min) * PXRandom());
    return result;
}


// ---------------------------------------- 
//               Vector 2 
// ---------------------------------------- 

inline v2
V2(f32 x, f32 y)
{
    v2 result;
    result.x = x;
    result.y = y;
    return result;
}

inline v2
V2(i32 x, i32 y)
{
    v2 result;
    result.x = (f32)x;
    result.y = (f32)y;
    return result;
}

inline v2
V2(u32 x, u32 y)
{
    v2 result;
    result.x = (f32)x;
    result.y = (f32)y;
    return result;
}

inline v2
operator+(v2 a, v2 b)
{
    v2 result;
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    return result;
}

inline v2
operator+=(v2 &a, v2 b)
{
    a = a + b;
    return a;
}

inline v2
operator-(v2 a)
{
  v2 result;
  result.x = -a.x;
  result.y = -a.y;
  return result;
}

inline v2
operator-(v2 a, v2 b)
{
    v2 result;
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    return result;
}

inline v2
operator-=(v2 &a, v2 b)
{
    a = a - b;
    return a;
}

inline v2
operator*(f32 a, v2 b)
{
    v2 result = { a * b.x, a * b.y };
    return result;
}

inline v2
operator*(v2 a, f32 b)
{
    v2 result = b * a;
    return result;
}

inline v2
V2Zero()
{
    return (v2)({0.f, 0.f});
}

inline v2
V2One()
{
    return (v2)({1.f, 1.f});
}

inline v2
V2Up()
{
    return (v2)({0.f, 1.f});
}

inline v2
V2Down()
{
    return (v2)({0.f, -1.f});
}

inline v2
VRight()
{
    return (v2)({1.f, 0.f});
}

inline v2
V2Left()
{
    return (v2)({-1.f, 0.f});
}

inline v2
Perp(v2 v)
{
    v2 result = { -v.y, v.x };
    return result;
}

inline f32
LengthSq(v2 v)
{
    f32 result = v.x * v.x + v.y + v.y;
    return result;
}

inline f32
Length(v2 v)
{
    f32 result = PXSqrt(LengthSq(v));
    return result;
}

inline b8
VecIsZero(v2 v)
{
    b8 result = v.x == 0.f && v.y == 0.f;
    return result;
}

inline v2
Normalize(v2 v)
{
    v2 result = v * (1.0f / Length(v));
    return result;
}

inline v2
Lerp(f32 t, v2 a, v2 b)
{
    v2 result = (1-t)*a + t*b;
    return result;
}

inline f32
Dot(v2 a, v2 b)
{
    f32 result = a.x * b.x + a.y * b.y;
    return result;
}

// ---------------------------------------- 
//               Vector 3 
// ---------------------------------------- 

inline v3
V3(f32 x, f32 y, f32 z)
{
    v3 result;
    result.x = x;
    result.y = y;
    result.z = z;
    return result;
}

inline v3
V3(i32 x, i32 y, i32 z)
{
    v2 result;
    result.x = (f32)x;
    result.y = (f32)y;
    result.z = (f32)z;
    return result;
}

inline v3
V3(u32 x, u32 y, u32 z)
{
    v2 result;
    result.x = (f32)x;
    result.y = (f32)y;
    result.y = (f32)z;
    return result;
}

inline v3
V3(v4 v)
{
    return (v3)({v.x, v.y, v.z});
}

inline v3
operator+(v3 a, v3 b)
{
    v3 result;
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    result.z = a.z + b.z;
    return result;
}

inline v3
operator+=(v3 &a, v3 b)
{
    a = a + b;
    return a;
}

inline v3
operator-(v3 a)
{
  v3 result;
  result.x = -a.x;
  result.y = -a.y;
  result.z = -a.z;
  return result;
}

inline v3
operator-(v3 a, v3 b)
{
    v3 result;
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    result.y = a.z - b.z;
    return result;
}

inline v3
operator-=(v3 &a, v3 b)
{
    a = a - b;
    return a;
}

inline v3
operator*(f32 a, v3 b)
{
    v3 result = { a * b.x, a * b.y, a * b.z };
    return result;
}

inline v3
operator*(v3 a, f32 b)
{
    v3 result = b * a;
    return result;
}

inline v3
V3Zero()
{
    return (v3)({0.f, 0.f, 0.f});
}

inline v3
V3One()
{
    return (v3)({1.f, 1.f, 1.f});
}

inline v3
V3Up()
{
    return (v3)({0.f, 1.f, 0.f});
}

inline v3
V3Down()
{
    return (v3)({0.f, -1.f, 0.f});
}

inline v3
V3Right()
{
    return (v3)({1.f, 0.f, 0.f});
}

inline v3
V3Left()
{
    return (v3)({-1.f, 0.f, 0.f});
}

inline v3
V3Forward()
{
    return (v3)({0.f, 0.f, 1.f});
}

inline v3
V3Backward()
{
    return (v3)({0.f, 0.f, 1.f});
}

inline f32
LengthSq(v3 v)
{
    f32 result = v.x * v.x + v.y + v.y + v.z * v.z;
    return result;
}

inline f32
Length(v3 v)
{
    f32 result = PXSqrt(LengthSq(v));
    return result;
}

inline b8
VecIsZero(v3 v)
{
    b8 result = v.x == 0.f && v.y == 0.f && v.z == 0.f;
    return result;
}

inline v3
Normalize(v3 v)
{
    v3 result = v * (1.0f / Length(v));
    return result;
}

inline v3
Lerp(f32 t, v3 a, v3 b)
{
    v3 result = (1-t)*a + t*b;
    return result;
}

inline f32
Dot(v3 a, v3 b)
{
    f32 result = a.x * b.x + a.y * b.y + a.z * b.z;
    return result;
}

inline v3
Cross(v3 a, v3 b)
{
    v3 result;
    result.x = a.y * b.z - a.z * b.y;
    result.y = a.z * b.x - a.x * b.z; // NOTE: swapped to avoid extra '-'
    result.z = a.x * b.y - a.y * b.x;
    return result;
}

// ---------------------------------------- 
//               Vector 4 
// ---------------------------------------- 

inline v4
V4(f32 x, f32 y, f32 z, f32 w)
{
    v4 result;
#ifdef PXUSE_SIMD
    result.data = __mm_setr_ps(x, y, z, w);
#else
    result.x = x;
    result.y = y;
    result.z = z;
#endif
    return result;
}

inline v4
V4(i32 x, i32 y, i32 z, i32 w)
{
    v4 result;
#ifdef PXUSE_SIMD
    result.data = __mm_setr_ps((i32)x, (i32)y, (i32)z, (i32)w);
#else
    result.x = (f32)x;
    result.y = (f32)y;
    result.z = (f32)z;
#endif
    return result;
}

inline v4
V4(u32 x, u32 y, u32 z, u32 w)
{
    v4 result;
#ifdef PXUSE_SIMD
    result.data = __mm_setr_ps((u32)x, (u32)y, (u32)z, (u32)w);
#else
    result.x = (f32)x;
    result.y = (f32)y;
    result.y = (f32)z;
#endif
    return result;
}

inline v4
V4(v3 v)
{
    v4 result;
    result.x = v.x;
    result.y = v.y;
    result.y = v.z;
    result.w = 0.f;
    return result;
}

inline v4
V4H(v3 v)
{
    v4 result;
    result.x = v.x;
    result.y = v.y;
    result.y = v.z;
    result.w = 1.f;
    return result;
}

inline v4
operator+(v4 a, v4 b)
{
    v4 result;
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    result.z = a.z + b.z;
    result.w = a.w + b.w;
    return result;
}

inline v4
operator+=(v4 &a, v4 b)
{
    a = a + b;
    return a;
}

inline v4
operator-(v4 a)
{
  v4 result;
  result.x = -a.x;
  result.y = -a.y;
  result.z = -a.z;
  result.w = -a.w;
  return result;
}

inline v4
operator-(v4 a, v4 b)
{
    v4 result;
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    result.y = a.z - b.z;
    result.w = a.z - b.w;
    return result;
}

inline v4
operator-=(v4 &a, v4 b)
{
    a = a - b;
    return a;
}

inline v4
operator*(f32 a, v4 b)
{
    v4 result = { a * b.x, a * b.y, a * b.z, a * b.w };
    return result;
}

inline v4
operator*(v4 a, f32 b)
{
    v4 result = b * a;
    return result;
}

inline f32
LengthSq(v4 v)
{
    f32 result = v.x * v.x + v.y + v.y + v.z * v.z + v.w * v.w;
    return result;
}

inline f32
Length(v4 v)
{
    f32 result = PXSqrt(LengthSq(v));
    return result;
}

inline b8
VecIsZero(v4 v)
{
    b8 result = v.x == 0.f && v.y == 0.f && v.z == 0.f && v.w == 0.f;
    return result;
}

inline v4
Normalize(v4 v)
{
    v4 result = v * (1.0f / Length(v));
    return result;
}

inline v4
Lerp(f32 t, v4 a, v4 b)
{
    v4 result = (1-t)*a + t*b;
    return result;
}

inline f32
Dot(v4 a, v4 b)
{
    f32 result = a.x * b.x + a.y * b.y + a.z * b.z + z.w * b.w;
    return result;
}

inline v4
UnHomo(v4 v)
{
    v4 result = v / v.w;
    return result;
}

