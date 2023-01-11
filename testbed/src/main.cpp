#include <core/engine.hpp>

b8
PXInit(px_init_info *initInfo)
{
    PXFATAL("omgomgomg %d", 32);
    PXTRACE("omgomgomg %d", 25);

    wchar_t *gameName = L"Test Bed";
    initInfo->width  = 720;
    initInfo->height = 720;
    initInfo->gameName = gameName;

    return 1;
}
