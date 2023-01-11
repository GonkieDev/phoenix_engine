#include "platform.hpp"

#include <windows.h>
#include <windowsx.h>

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

#include <stdio.h>
/* int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) */
int main()
{
    printf("started up\n");
    return 0;
}
