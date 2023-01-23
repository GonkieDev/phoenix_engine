#pragma once

#include <defines.hpp>

#ifdef _DEBUG

#ifdef _MSC_VER
#include <intrin.h>
#define debugBreak() __debugbreak()
#else
#define debugBreak() __buildin_trap()
#endif 

void ReportAssertionFailure(char *expression, char *message, char *file, i32 line);

#define PX_ASSERT(expr)                                                     \
    {                                                                       \
        if (expr) {}                                                        \
        else                                                                \
        {                                                                   \
            ReportAssertionFailure(#expr, "", __FILE__, __LINE__);          \
            debugBreak();                                                   \
        }                                                                   \
    }

#define PX_ASSERT_MSG(expr, message)                                        \
    {                                                                       \
        if (expr) {}                                                        \
        else                                                                \
        {                                                                   \
            ReportAssertionFailure(#expr, message, __FILE__, __LINE__);     \
            debugBreak();                                                   \
        }                                                                   \
    }

#else // _DEBUG

#define PX_ASSERT_MSG(expr, message)
#define PX_ASSERT(expr)

#endif // !_DEBUG

