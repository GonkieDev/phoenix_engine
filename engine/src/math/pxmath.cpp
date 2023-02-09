#include <math/pxmath.hpp>
#include <math.h>

// xxoptimisationxx : SIMD 

// ---------------------------------------- 
//              General Math
// ---------------------------------------- 

inline f32
DegToRad(f32 degrees)
{
    return degrees * PX_DEG2RAD_COEF;
}

inline f32
RadToDeg(f32 rads)
{
    return rads * PX_RAD2DEG_COEF;
}


inline f32
PXSin(f32 x)
{
    return sinf(x);
}

inline f32
PXCos(f32 x)
{
    return cosf(x);
}

inline f32
PXTan(f32 x)
{
    return tanf(x);
}

inline f32
PXASin(f32 x)
{
    return asinf(x);
}

inline f32
PXACos(f32 x)
{
    return acosf(x);
}

inline f32
PXSqrt(f32 x)
{
    return sqrtf(x);
}

inline f32
PXAbs(f32 x)
{
    return fabsf(x);
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

// TODO 
#if 0
inline i32
PXRandomI32()
{ }

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
#endif


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
    v2 result;
    result.x = a * b.x;
    result.y = a * b.y;
    return result;
}

inline v2
operator*(v2 a, f32 b)
{
    v2 result = b * a;
    return result;
}

inline v2
operator*=(v2 &a, f32 b)
{
    a = a * b;
    return a;
}

inline v2
operator/(f32 a, v2 b)
{
    v2 result = {{ a / b.x, a / b.y }};
    return result;
}

inline v2
operator/(v2 a, f32 b)
{
    v2 result = b / a;
    return result;
}

inline v2
operator/=(v2 &a, f32 b)
{
    a = a / b;
    return a;
}

inline v2
V2Zero()
{
    return {{0.f, 0.f}};
}

inline v2
V2One()
{
    return (v2){{1.f, 1.f}};
}

inline v2
V2Up()
{
    return (v2){{0.f, 1.f}};
}

inline v2
V2Down()
{
    return (v2){{0.f, -1.f}};
}

inline v2
VRight()
{
    return (v2){{1.f, 0.f}};
}

inline v2
V2Left()
{
    return (v2){{-1.f, 0.f}};
}

inline v2
Perp(v2 v)
{
    v2 result = {{ -v.y, v.x }};
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
    f32 l = Length(v);
    v2 result = l == 0 ? v : v * (1.0f / l);
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
    v3 result;
    result.x = (f32)x;
    result.y = (f32)y;
    result.z = (f32)z;
    return result;
}

inline v3
V3(u32 x, u32 y, u32 z)
{
    v3 result;
    result.x = (f32)x;
    result.y = (f32)y;
    result.y = (f32)z;
    return result;
}

inline v3
V3(v4 v)
{
    return (v3){{v.x, v.y, v.z}};
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
    v3 result = {{ a * b.x, a * b.y, a * b.z }};
    return result;
}

inline v3
operator*(v3 a, f32 b)
{
    v3 result = b * a;
    return result;
}

inline v3
operator*=(v3 &a, f32 b)
{
    a = a * b;
    return a;
}

inline v3
operator/(f32 a, v3 b)
{
    v3 result = {{ a / b.x, a / b.y, a / b.z }};
    return result;
}

inline v3
operator/(v3 a, f32 b)
{
    v3 result = b / a;
    return result;
}

inline v3
operator/=(v3 &a, f32 b)
{
    a = a / b;
    return a;
}


inline v3
V3Zero()
{
    return (v3){{0.f, 0.f, 0.f}};
}

inline v3
V3One()
{
    return (v3){{1.f, 1.f, 1.f}};
}

inline v3
V3Up()
{
    return (v3){{0.f, 1.f, 0.f}};
}

inline v3
V3Down()
{
    return (v3){{0.f, -1.f, 0.f}};
}

inline v3
V3Right()
{
    return (v3){{1.f, 0.f, 0.f}};
}

inline v3
V3Left()
{
    return (v3){{-1.f, 0.f, 0.f}};
}

inline v3
V3Forward()
{
    return (v3){{0.f, 0.f, -1.f}};
}

inline v3
V3Backward()
{
    return (v3){{0.f, 0.f, 1.f}};
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
    f32 l = Length(v);
    v3 result = l == 0 ? v : v * (1.0f / l);
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
    v4 result = {{ a * b.x, a * b.y, a * b.z, a * b.w }};
    return result;
}

inline v4
operator*(v4 a, f32 b)
{
    v4 result = b * a;
    return result;
}

inline v4
operator*=(v4 &a, f32 b)
{
    a = a * b;
    return a;
}

inline v4
operator/(f32 a, v4 b)
{
    v4 result = {{ a / b.x, a / b.y, a / b.z, a / b.w }};
    return result;
}

inline v4
operator/(v4 a, f32 b)
{
    v4 result = b / a;
    return result;
}

inline v4
operator/=(v4 &a, f32 b)
{
    a = a / b;
    return a;
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
    f32 l = Length(v);
    v4 result = l == 0 ? v : v * (1.0f / l);
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
    f32 result = a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
    return result;
}

inline v4
UnHomo(v4 v)
{
    v4 result = v / v.w;
    return result;
}


// ---------------------------------------- 
//               Matrices 
// ---------------------------------------- 

inline m4
M4Zeros()
{
    m4 result;
    result.data[0]  = 0.f;
    result.data[1]  = 0.f;
    result.data[2]  = 0.f;
    result.data[3]  = 0.f;
    result.data[4]  = 0.f;
    result.data[5]  = 0.f;
    result.data[6]  = 0.f;
    result.data[7]  = 0.f;
    result.data[8]  = 0.f;
    result.data[9]  = 0.f;
    result.data[10] = 0.f;
    result.data[11] = 0.f;
    result.data[12] = 0.f;
    result.data[13] = 0.f;
    result.data[14] = 0.f;
    result.data[15] = 0.f;
    return result;
}

inline m4 
M4Indentity()
{
    m4 result;

    result.data[0]  = 1.f;
    result.data[1]  = 0.f;
    result.data[2]  = 0.f;
    result.data[3]  = 0.f;

    result.data[4]  = 0.f;
    result.data[5]  = 1.f;
    result.data[6]  = 0.f;
    result.data[7]  = 0.f;

    result.data[8]  = 0.f;
    result.data[9]  = 0.f;
    result.data[10] = 1.f;
    result.data[11] = 0.f;

    result.data[12] = 0.f;
    result.data[13] = 0.f;
    result.data[14] = 0.f;
    result.data[15] = 1.f;

    return result;
}

inline m4
operator*(m4 a, m4 b)
{
    m4 result;

    f32 *ap = a.data; 
    f32 *bp = b.data; 
    f32 *dstP = result.data;

    for (u32 i = 0; i < 4; i++)
    {
        for (u32 j = 0; j < 4; j++)
        {
            *dstP = 
                ap[0] * bp[0 + j] + 
                ap[1] * bp[4 + j] + 
                ap[2] * bp[8 + j] + 
                ap[3] * bp[12 + j];
            dstP++;
        }
        ap += 4;
    }

    return result;
}

inline m4
operator*=(m4 &a, m4 b)
{
    a = a * b;
    return a;
}

inline m4
Ortho(f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far)
{
    m4 result = M4Zeros();

    f32 rl = right - left;
    f32 tb = top - bottom;
    f32 fn = far - near;

    result.data[0]  = 2.0f / (rl);
    result.data[5]  = 2.0f / (tb);
    result.data[10] = -2.0f / (fn);
    result.data[15] = 1.f;

    result.data[3]  = - (right + left) / (rl);
    result.data[7]  = - (top + bottom) / (tb);
    result.data[11] = - (far + near) / (fn);

    return result;
}

inline m4
Perspective(f32 fovRads, f32 aspectRatio, f32 near, f32 far)
{
    m4 result = M4Zeros();
    f32 halfTanFov = PXTan(fovRads * 0.5f);

    result.data[0] = 1.f / (aspectRatio * halfTanFov);
    result.data[5] = 1.f / halfTanFov;
    result.data[10] = -((far + near) / (far - near));
    result.data[11] = -1.f;
    result.data[14] = -((2.f * far * near) / (far - near));

    return result;
}

// NOTE: eyePos is the position of the 'viewer'/'camera'
inline m4
LookAt(v3 eyePos, v3 targetPos, v3 upDir)
{
    m4 result;

    v3 zAxis = Normalize(targetPos - eyePos);
    v3 xAxis = Normalize(Cross(zAxis, upDir));
    v3 yAxis = Cross(xAxis, zAxis);

    result.data[0] = xAxis.x;
    result.data[1] = yAxis.x;
    result.data[2] = -zAxis.x;
    result.data[3] = 0.f;

    result.data[4] = xAxis.y;
    result.data[5] = yAxis.y;
    result.data[6] = -zAxis.y;
    result.data[7] = 0.f;

    result.data[8] = xAxis.z;
    result.data[9] = yAxis.z;
    result.data[10] = -zAxis.z;
    result.data[11] = 0.f;

    result.data[12] = -Dot(xAxis, eyePos);
    result.data[13] = -Dot(yAxis, eyePos);
    result.data[14] = -Dot(zAxis, eyePos);
    result.data[15] = 1.f;

    return result;
}

inline m4
M4Transpose(m4 mat)
{
    m4 result;

    result.data[0] = mat.data[0];
    result.data[1] = mat.data[4];
    result.data[2] = mat.data[8];
    result.data[3] = mat.data[12];
    result.data[4] = mat.data[1];
    result.data[5] = mat.data[5];
    result.data[6] = mat.data[9];
    result.data[7] = mat.data[13];
    result.data[8] = mat.data[2];
    result.data[9] = mat.data[6];
    result.data[10] = mat.data[10];
    result.data[11] = mat.data[14];
    result.data[12] = mat.data[3];
    result.data[13] = mat.data[7];
    result.data[14] = mat.data[11];
    result.data[15] = mat.data[15];

    return result;
}

inline m4
M4Inverse(m4 mat)
{
    f32* m = mat.data;

    f32 t0 = m[10] * m[15];
    f32 t1 = m[14] * m[11];
    f32 t2 = m[6] * m[15];
    f32 t3 = m[14] * m[7];
    f32 t4 = m[6] * m[11];
    f32 t5 = m[10] * m[7];
    f32 t6 = m[2] * m[15];
    f32 t7 = m[14] * m[3];
    f32 t8 = m[2] * m[11];
    f32 t9 = m[10] * m[3];
    f32 t10 = m[2] * m[7];
    f32 t11 = m[6] * m[3];
    f32 t12 = m[8] * m[13];
    f32 t13 = m[12] * m[9];
    f32 t14 = m[4] * m[13];
    f32 t15 = m[12] * m[5];
    f32 t16 = m[4] * m[9];
    f32 t17 = m[8] * m[5];
    f32 t18 = m[0] * m[13];
    f32 t19 = m[12] * m[1];
    f32 t20 = m[0] * m[9];
    f32 t21 = m[8] * m[1];
    f32 t22 = m[0] * m[5];
    f32 t23 = m[4] * m[1];

    m4 out_matrix;
    f32* o = out_matrix.data;

    o[0] = (t0 * m[5] + t3 * m[9] + t4 * m[13]) - (t1 * m[5] + t2 * m[9] + t5 * m[13]);
    o[1] = (t1 * m[1] + t6 * m[9] + t9 * m[13]) - (t0 * m[1] + t7 * m[9] + t8 * m[13]);
    o[2] = (t2 * m[1] + t7 * m[5] + t10 * m[13]) - (t3 * m[1] + t6 * m[5] + t11 * m[13]);
    o[3] = (t5 * m[1] + t8 * m[5] + t11 * m[9]) - (t4 * m[1] + t9 * m[5] + t10 * m[9]);

    f32 d = 1.0f / (m[0] * o[0] + m[4] * o[1] + m[8] * o[2] + m[12] * o[3]);

    o[0] = d * o[0];
    o[1] = d * o[1];
    o[2] = d * o[2];
    o[3] = d * o[3];
    o[4] = d * ((t1 * m[4] + t2 * m[8] + t5 * m[12]) - (t0 * m[4] + t3 * m[8] + t4 * m[12]));
    o[5] = d * ((t0 * m[0] + t7 * m[8] + t8 * m[12]) - (t1 * m[0] + t6 * m[8] + t9 * m[12]));
    o[6] = d * ((t3 * m[0] + t6 * m[4] + t11 * m[12]) - (t2 * m[0] + t7 * m[4] + t10 * m[12]));
    o[7] = d * ((t4 * m[0] + t9 * m[4] + t10 * m[8]) - (t5 * m[0] + t8 * m[4] + t11 * m[8]));
    o[8] = d * ((t12 * m[7] + t15 * m[11] + t16 * m[15]) - (t13 * m[7] + t14 * m[11] + t17 * m[15]));
    o[9] = d * ((t13 * m[3] + t18 * m[11] + t21 * m[15]) - (t12 * m[3] + t19 * m[11] + t20 * m[15]));
    o[10] = d * ((t14 * m[3] + t19 * m[7] + t22 * m[15]) - (t15 * m[3] + t18 * m[7] + t23 * m[15]));
    o[11] = d * ((t17 * m[3] + t20 * m[7] + t23 * m[11]) - (t16 * m[3] + t21 * m[7] + t22 * m[11]));
    o[12] = d * ((t14 * m[10] + t17 * m[14] + t13 * m[6]) - (t16 * m[14] + t12 * m[6] + t15 * m[10]));
    o[13] = d * ((t20 * m[14] + t12 * m[2] + t19 * m[10]) - (t18 * m[10] + t21 * m[14] + t13 * m[2]));
    o[14] = d * ((t18 * m[6] + t23 * m[14] + t15 * m[2]) - (t22 * m[14] + t14 * m[2] + t19 * m[6]));
    o[15] = d * ((t22 * m[10] + t16 * m[2] + t21 * m[6]) - (t20 * m[6] + t23 * m[10] + t17 * m[2]));

    return out_matrix;
}

inline m4
M4Trans(v3 offset)
{
    m4 result = M4Indentity();
    result.data[12] = offset.x;
    result.data[13] = offset.y;
    result.data[14] = offset.z;

    return result;
}

inline v3
M4GetTrans(m4 m)
{
    return (v3){{m.data[12], m.data[13], m.data[14]}};
}

inline m4
M4Scale(v3 scale)
{
    m4 result = M4Zeros();

    result.data[0]  = scale.x;
    result.data[5]  = scale.y;
    result.data[10] = scale.z;
    result.data[15] = 1.f;

    return result;
}

inline m4
M4EulerX(f32 rads)
{
    m4 result = M4Indentity();
    f32 c = PXCos(rads);
    f32 s = PXSin(rads);

    result.data[5]  = c;
    result.data[6]  = s;
    result.data[9]  = -s;
    result.data[10] = c;
    return result;
}

inline m4
M4EulerY(f32 rads)
{
    m4 result = M4Indentity();
    f32 c = PXCos(rads);
    f32 s = PXSin(rads);

    result.data[0]  = c;
    result.data[2]  = -s;
    result.data[8]  = s;
    result.data[10] = c;
    return result;
}

inline m4
M4EulerZ(f32 rads)
{
    m4 result = M4Indentity();

    f32 c = PXCos(rads);
    f32 s = PXSin(rads);

    result.data[0] = c;
    result.data[1] = s;
    result.data[4] = -s;
    result.data[5] = c;
    return result;
}

inline m4
M4EulerXYZ(v3 r)
{
    m4 result = M4EulerX(r.x) * M4EulerY(r.y) * M4EulerZ(r.z);
    return result;
}


// ---------------------------------------- 
//             Quaternions 
// ---------------------------------------- 

inline quat
operator+(quat a, quat b)
{
    quat result;
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    result.z = a.z + b.z;
    result.w = a.w + b.w;
    return result;
}

inline quat
operator+=(quat &a, quat b)
{
    a = a + b;
    return a;
}

inline quat
operator-(quat a, quat b)
{
    quat result;
    result.x = a.x - b.x;
    result.x = a.y - b.y;
    result.x = a.z - b.z;
    result.x = a.w - b.w;
    return result;
}

inline quat
operator-=(quat &a, quat b)
{
    a = a - b;
    return a;
}

inline quat
operator*(quat a, quat b)
{
    quat result;

    result.x =
        a.x * b.w +
        a.y * b.z -
        a.z * b.y +
        a.w * b.x;

    result.y =
        -a.x * b.z +
        a.y * b.w +
        a.z * b.x +
        a.w * b.y;

    result.z =
        a.x * b.y -
        a.y * b.x +
        a.z * b.w +
        a.w * b.z;

    result.w =
        -a.x * b.x -
        a.y * b.y -
        a.z * b.z +
        a.w * b.w;

    return result;
}

inline quat
operator*=(quat &a, quat b)
{
    a = a * b;
    return a;
}

inline quat
QuatIndentity()
{
    return (quat){{0.f, 0.f, 0.f, 1.f}};
}

inline f32
QuatLength(quat q)
{
    f32 l = PXSqrt(
        q.x * q.x +
        q.y * q.y + 
        q.z * q.z +
        q.w * q.w);

    return l;
}

inline quat
QuatNormalize(quat q)
{
    f32 l = QuatLength(q);
    return (quat){{
        q.x / l,
        q.y / l,
        q.z / l,
        q.w / l}};
}

inline quat
QuatConjugate(quat q)
{
    return (quat){{-q.x, -q.y, -q.z, q.w}};
}

inline quat
QuatInverse(quat q)
{
    // xxoptimisationxx can the hamilton be used instead of this ?
    return QuatNormalize(QuatConjugate(q));
}

inline m4
QuatToM4(quat q)
{
    // https://stackoverflow.com/questions/1556260/convert-quaternion-rotation-to-rotation-matrix

    m4 result;

    quat n = QuatNormalize(q);

    result.data[0] = 1.0f - 2.0f * n.y * n.y - 2.0f * n.z * n.z;
    result.data[1] = 2.0f * n.x * n.y - 2.0f * n.z * n.w;
    result.data[2] = 2.0f * n.x * n.z + 2.0f * n.y * n.w;

    result.data[4] = 2.0f * n.x * n.y + 2.0f * n.z * n.w;
    result.data[5] = 1.0f - 2.0f * n.x * n.x - 2.0f * n.z * n.z;
    result.data[6] = 2.0f * n.y * n.z - 2.0f * n.x * n.w;

    result.data[8] = 2.0f * n.x * n.z - 2.0f * n.y * n.w;
    result.data[9] = 2.0f * n.y * n.z + 2.0f * n.x * n.w;
    result.data[10] = 1.0f - 2.0f * n.x * n.x - 2.0f * n.y * n.y;

    return result;
}

inline quat
QuatFromAxisEuler(v3 axis, f32 angleInRads, b8 normalize = 1)
{
    f32 halfAngle = 0.5f * angleInRads;
    f32 s = PXSin(halfAngle);
    f32 c = PXCos(halfAngle);

    quat q = (quat){{s * axis.x, s * axis.y, s * axis.z, c}};

    if (normalize)
    {
        return QuatNormalize(q);
    }

    return q;
}

inline f32
Dot(quat a, quat b)
{
    f32 result =
        a.x * b.x +
        a.y * b.y +
        a.z * b.z +
        a.w * b.w;

    return result;
}

inline quat
Slerp(quat a, quat b, f32 t)
{
    quat result;

    a = QuatNormalize(a);
    b = QuatNormalize(b);

    f32 dot = Dot(a, b);

    // If dot is negative, slerp won't take shortest path.
    // Can be fixed by negating dot result and quaternion
    if (dot < 0.0f)
    {
        b.x = -b.x;
        b.y = -b.y;
        b.z = -b.z;
        b.w = -b.w;
        dot = -dot;
    }

    f32 threshhold = 0.9995f; 
    if (dot > threshhold)
    {
        result = (quat){{
            a.x + ((b.x - a.x) * t),
            a.y + ((b.y - a.y) * t),
            a.z + ((b.z - a.z) * t),
            a.w + ((b.w - a.w) * t) }};

        result = QuatNormalize(result);
        return result;
    }

    f32 theta0 = PXACos(dot);           // angle between input vectors
    f32 theta  = theta0 * t;            // angle between a and result
    f32 sinTheta0 = PXSin(theta0);
    f32 sinTheta  = PXSin(theta);

    f32 s0 = PXCos(theta) - dot * sinTheta / sinTheta0; // == sin(theta0 - theta) / sin(theta0)
    f32 s1 = sinTheta / sinTheta0;

    return (quat){{
        (a.x * s0) + (b.x * s1),
        (a.y * s0) + (b.y * s1),
        (a.z * s0) + (b.z * s1),
        (a.w * s0) + (b.w * s1) }};
}
