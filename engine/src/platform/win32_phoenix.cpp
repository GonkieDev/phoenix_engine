#include "platform.hpp"

#include "../core/logger.cpp"

#include <windows.h>
#include <windowsx.h>

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

#include <stdio.h>
/* int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) */
int main()
{
    printf("started up\n");
    return 0;
}
