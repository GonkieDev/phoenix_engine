#include "../defines.hpp"
#include "../core/asserts.h"
#include "../core/logger.hpp"
#include "../core/memory_arenas.hpp"
#include "../core/engine.hpp"
#include "platform.hpp"

// Includes for unity build
#include "../core/engine.cpp"

#include <vulkan/vulkan.h>

#include <windows.h>
#include <xinput.h>

struct internal_state
{
    HWND hwnd;
    HINSTANCE hinstance;
    int nCmdShow;
    VkSurfaceKHR surface;
    f64 clockFrequency;
    LARGE_INTEGER startTime;
};

//
// Function declarations
//

// Functions that are used in this file only
LRESULT CALLBACK win32WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
#if 0
internal b32     win32SetupEngineMemory(game_memory *memory);

// Input
internal void        win32ProcessGamepadInput(game_input *gameInput);
internal inline void win32ProcessKeyboardMessage(button_state *key, b32 isDown);
internal void        win32ProcessMessageQueue(b32 *engineRunning, game_input *input);
#endif

// XInput
internal inline void win32LoadXInput();

#define X_INPUT_GET_STATE(name) DWORD WINAPI name(DWORD /*dwUserIndex*/, XINPUT_STATE * /*pState*/)
typedef X_INPUT_GET_STATE(x_input_get_state);
X_INPUT_GET_STATE(XInputGetStateStub)
{
    return 0;
}
global_var x_input_get_state *XInputGetState_ = XInputGetStateStub;
#define XInputGetState XInputGetState_

#define X_INPUT_SET_STATE(name) DWORD WINAPI name(DWORD /*dwUserIndex*/, XINPUT_VIBRATION * /*pVibration*/)
typedef X_INPUT_SET_STATE(x_input_set_state);
X_INPUT_SET_STATE(XInputSetStateStub)
{
    return 0;
}
global_var x_input_set_state *XInputSetState_ = XInputSetStateStub;
#define XInputSetState XInputSetState_

int WINAPI
wWinMain(_In_     HINSTANCE hInstance,
         _In_opt_ HINSTANCE /* hPrevInstance */,
         _In_     LPWSTR    /* lpCmdLine */,
         _In_     int       nCmdShow)
{
    internal_state internalState = {};
    internalState.hinstance = hInstance;
    internalState.nCmdShow = nCmdShow;

    // Timer setup
    {
        LARGE_INTEGER frequency;
        QueryPerformanceFrequency(&frequency);
        internalState.clockFrequency = 1.0 / (f64)frequency.QuadPart; 
        QueryPerformanceCounter(&internalState.startTime);
    }

    platform_state platformState = {};
    platformState.internalState = &internalState;

    GameMain(&platformState);

    // Cleanup
    ShutdownLogging();

    return 0;
}

internal b8
PlatformInit(u32 width, u32 height, wchar_t *applicationName, platform_state_ptr platformState)
{
    InitLogging();
    win32LoadXInput();

    internal_state *internalState = (internal_state*)platformState->internalState;

    // Initialize the window class.
    WNDCLASSEX windowClass    = {};
    windowClass.cbSize        = sizeof(WNDCLASSEX);
    windowClass.style         = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc   = win32WindowProc;
    windowClass.hInstance     = internalState->hinstance;
    /* windowClass.hIcon         = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION)); */
    windowClass.hCursor       = LoadCursor(0, IDC_ARROW);
    windowClass.lpszClassName = L"PhoenixEngineWindowClass";
    windowClass.cbWndExtra    = sizeof(LONG_PTR);

    if (!RegisterClassEx(&windowClass))
    {
        MessageBoxA(0, "Window registration failed", "Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    u32 windowWidth  = width;
    u32 windowHeight = height;

    u32 windowStyle   = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX |
                        WS_THICKFRAME;
    u32 windowExStyle = WS_EX_APPWINDOW;

    // Obtain the size of the border.
    RECT borderRect = {0, 0, 0, 0};
    AdjustWindowRectEx(&borderRect, windowStyle, 0, windowExStyle);

    // Grow by the size of the OS border.
    windowWidth  += borderRect.right  - borderRect.left;
    windowHeight += borderRect.bottom - borderRect.top;

    internalState->hwnd = CreateWindowEx(
        windowExStyle, windowClass.lpszClassName, applicationName, windowStyle,
        CW_USEDEFAULT, CW_USEDEFAULT, windowWidth, windowHeight,
        0, 0, internalState->hinstance, 0
    );

    if (!internalState->hwnd)
    {
        MessageBoxA(0, "Window creation failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);

        PXFATAL("Window creation failed!");
        return 0;
    }

    SetWindowLongPtr(internalState->hwnd, 0, (LONG_PTR)(&platformState));

    ShowWindow(internalState->hwnd, internalState->nCmdShow);

    return 1;
}

internal inline void
win32LoadXInput()
{
    HMODULE XInputLib = LoadLibrary(L"xinput1_4.dll");
    if (XInputLib)
    {
        XInputGetState = (x_input_get_state *)GetProcAddress(XInputLib, "XInputGetState");
        XInputSetState = (x_input_set_state *)GetProcAddress(XInputLib, "XInputSetState");
    }
    else
    {
        /* win32DebugLogFile(LOG_PLATFORM_FILE, "ERROR:: XInput DLL Failed to be loaded!!!!"); */
    }
}

void *
PlatformAllocateMemory(u64 size, b8 aligned)
{
#ifdef INTERNAL_BUILD
        LPVOID baseAddress = (LPVOID)GIBIBYTES((u64)2000);
#else
        LPVOID  baseAddress = 0;
#endif
    void *result = VirtualAlloc(baseAddress, size, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);

    return result;
}

void PlatformFreeMemory(void *memory, b8 aligned)
{
    if (memory)
    {
        VirtualFree(memory, 0, MEM_RELEASE);
    }
}

internal void
PlatformDebugOutput(char *message)
{
    OutputDebugStringA((LPCSTR)message);
}

LRESULT CALLBACK 
win32WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    /* platform_state *platformState = (platform_state*)GetWindowLongPtr(hwnd, 0); */
    switch (uMsg)
    {
        case WM_ERASEBKGND:
            // NOTE: erasing will be handled by application to prevent flicker
            return 1;

        case WM_CLOSE:
            // TODO: fire event to quit application
            /* platformState->quitRequest = 1; */
            return 0;
        case WM_DESTROY:
            /* platformState->quitRequest = 1; */
            PostQuitMessage(0);
            return 0;

        case WM_SIZE:
        {
            /* u32 width  = (u32)LOWORD(lParam); */
            /* u32 height = (u32)HIWORD(lParam); */

            // TODO: fire event to resize screen

            /* platformState->windowResized = 1; */
            /* platformState->width = width; */
            /* platformState->height = height; */
        } break;

        case WM_SYSKEYUP:
        case WM_SYSKEYDOWN:
        case WM_KEYUP:
        case WM_KEYDOWN:
        {
            // TODO: input processing
        } break;

        case WM_MOUSEMOVE:
        {
            /* i32 xMousePos = GET_X_LPARAM(lParam); */
            /* i32 yMousePos = GET_Y_LPARAM(lParam); */
            // TODO: input processing
        } break;
        case WM_LBUTTONUP:
        case WM_LBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_RBUTTONDOWN:
        case WM_MBUTTONUP:
        case WM_MBUTTONDOWN:
        {
            // b8 pressed = msg == WM_LBUTTONDOWN || msg == WM_RBUTTONDOWN || msg == WM_MBUTTONDOWN;
            // TODO: input processing
        } break;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
