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
#include <vulkan/vulkan.h>

#include <windows.h>
#include <windowsx.h>

#include "platform.hpp"
#include "../core/engine.hpp"

#include "../core/logger.cpp"

#if 0
struct plaform_internal_state
{
    HINSTANCE hInstane;
    HWND      hwnd;
};

b8
PlatformStartup(platform_startup_info info, platform_state *platformState)
{
    return 1;
}
#endif

extern "C" int WINAPI
wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    InitLogging();

    GameInit();

    ShutdownLogging();

    return 0;
}
