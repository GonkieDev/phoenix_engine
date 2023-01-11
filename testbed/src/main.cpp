#include <core/logger.hpp>
#include <core/asserts.h>

#include <win32_phoenix.cpp>

int cum()
{
    PXFATAL("omgomgomg %d", 32);
    PXTRACE("omgomgomg %d", 25);

    PX_ASSERT_MSG(0, "cum");

    return 0;
}
