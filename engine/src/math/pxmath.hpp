#pragma once

#include <math/math_types.hpp>

#define PX_PI 3.14159265359f
#define PX_2PI 2.f * PX_PI
#define PX_HALF_PI 0.5f * PX_PI
#define PX_QUARTER_PI 0.25f * PX_PI
#define PX_SQRT_TWO 1.41421356237f
#define PX_DEG2RAD_COEF PX_PI / 180.f
#define PX_RAD2DEG_COEF 180.f / PX_PI

//
// NOTE: ABOUT MATRICES (for my own sake)
// Matices are stored contiguously in memory.
// Translation components are the 13th, 14th and 15th elements of the matrix.
// Use pre-multiplication.
// Resources for mind boggling confusion that I get sometimes:
// - https://www.scratchapixel.com/lessons/mathematics-physics-for-computer-graphics/geometry/row-major-vs-column-major-vector.html

