#include "../core/engine.hpp"

// Includes for unity build
#include "../core/logger.cpp"

#include <vulkan/vulkan.h>

#include <windows.h>
#include <xinput.h>

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
    InitLogging();

    px_init_info initInfo = {};
    PXInit(&initInfo);

    platform_state platformState = {};
    platformState.width  = initInfo.width;
    platformState.height = initInfo.height;

    win32LoadXInput();

    // Initialize the window class.
    WNDCLASSEX windowClass    = {};
    windowClass.cbSize        = sizeof(WNDCLASSEX);
    windowClass.style         = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc   = win32WindowProc;
    windowClass.hInstance     = hInstance;
    /* windowClass.hIcon         = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION)); */
    windowClass.hCursor       = LoadCursor(0, IDC_ARROW);
    windowClass.lpszClassName = L"PhoenixEngineWindowClass";
    windowClass.cbWndExtra    = sizeof(LONG_PTR);

    if (!RegisterClassEx(&windowClass))
    {
        MessageBoxA(0, "Window registration failed", "Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    u32 windowWidth  = initInfo.width;
    u32 windowHeight = initInfo.height;

    u32 windowStyle   = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX |
                        WS_THICKFRAME;
    u32 windowExStyle = WS_EX_APPWINDOW;

    // Obtain the size of the border.
    RECT borderRect = {0, 0, 0, 0};
    AdjustWindowRectEx(&borderRect, windowStyle, 0, windowExStyle);

    // Grow by the size of the OS border.
    windowWidth  += borderRect.right  - borderRect.left;
    windowHeight += borderRect.bottom - borderRect.top;

    HWND hwnd = CreateWindowEx(
        windowExStyle, windowClass.lpszClassName, initInfo.gameName, windowStyle,
        CW_USEDEFAULT, CW_USEDEFAULT, windowWidth, windowHeight,
        0, 0, hInstance, 0
    );

    if (!hwnd)
    {
        MessageBoxA(0, "Window creation failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);

        PXFATAL("Window creation failed!");
        return 0;
    }

    SetWindowLongPtr(hwnd, 0, (LONG_PTR)(&platformState));

    ShowWindow(hwnd, nCmdShow);

    // Cleanup
    ShutdownLogging();

    return 0;
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

LRESULT CALLBACK 
win32WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;
    platform_state *platformState = (platform_state*)GetWindowLongPtr(hwnd, 0);
    switch (uMsg)
    {
        // NOTE: Enable this once input has been finished to make sure we don't miss any buttons
#if 0
        case WM_SYSKEYUP:
        case WM_SYSKEYDOWN:
        case WM_KEYUP:
        case WM_KEYDOWN:
        {
            /* Assert(!"no keyboard message here") */
        } break;
#endif // !0

        case WM_QUIT:
        {
            platformState->quitRequest = 1;
            break;
        }
        case WM_DESTROY:
        {
            platformState->quitRequest = 1; // TODO: if window being destroyed we prolly want to do smtg else
            break;
        }
        
        case WM_CLOSE:
        {
            platformState->quitRequest = 1;
            break;
        } 

        case WM_SIZE:
        {
            u32 width  = (u32)LOWORD(lParam);
            u32 height = (u32)HIWORD(lParam);
            platformState->windowResized = 1;
            platformState->width = width;
            platformState->height = height;
        } break;

        default:
        {
            result = DefWindowProc(hwnd, uMsg, wParam, lParam);
        } break;
    }

    return result;
}

b8
PlatformPumpMessages(platform_state *platformState)
{

    return 1;
}
