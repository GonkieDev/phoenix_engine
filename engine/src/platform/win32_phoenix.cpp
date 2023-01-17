#include "../core/engine.hpp"

// Includes for unity build
#include "../core/engine.cpp"

#include <vulkan/vulkan.h>

#include <windows.h>
#include <xinput.h>

struct platform_state
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
PXAPI b32     win32SetupEngineMemory(game_memory *memory);

// Input
PXAPI void        win32ProcessGamepadInput(game_input *gameInput);
PXAPI inline void win32ProcessKeyboardMessage(button_state *key, b32 isDown);
PXAPI void        win32ProcessMessageQueue(b32 *engineRunning, game_input *input);
#endif

// XInput
PXAPI inline void win32LoadXInput();

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

#define WIN32_FAIL_EXIT 1
#define WIN32_SUCCESS_EXIT 0
int WINAPI
wWinMain(_In_     HINSTANCE hInstance,
         _In_opt_ HINSTANCE /* hPrevInstance */,
         _In_     LPWSTR    /* lpCmdLine */,
         _In_     int       nCmdShow)
{
    platform_state platformState = {};
    platformState.hinstance = hInstance;
    platformState.nCmdShow = nCmdShow;

#if _DEBUG
    AllocConsole();
    freopen("CONOUT$", "wb", stdout);
    freopen("CONOUT$", "wb", stderr);
#endif // !_DEBUG

    // Timer setup
    {
        LARGE_INTEGER frequency;
        QueryPerformanceFrequency(&frequency);
        platformState.clockFrequency = 1.0 / (f64)frequency.QuadPart; 
        QueryPerformanceCounter(&platformState.startTime);
    }

    engine_state engineState = {};
    engineState.platformState = &platformState;

    if (!PhoenixInit(&engineState))
        return WIN32_FAIL_EXIT;

    if (!PhoenixRun(&engineState))
        return WIN32_FAIL_EXIT;

    return WIN32_SUCCESS_EXIT;
}

PXAPI b8
PlatformInit(wchar_t *applicationName, engine_state *engineState)
{
    win32LoadXInput();

    platform_state *platformState = engineState->platformState;

    // Initialize the window class.
    WNDCLASSEX windowClass    = {};
    windowClass.cbSize        = sizeof(WNDCLASSEX);
    windowClass.style         = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc   = win32WindowProc;
    windowClass.hInstance     = platformState->hinstance;
    /* windowClass.hIcon         = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION)); */
    windowClass.hCursor       = LoadCursor(0, IDC_ARROW);
    windowClass.lpszClassName = L"PhoenixEngineWindowClass";
    windowClass.cbWndExtra    = sizeof(LONG_PTR);

    if (!RegisterClassEx(&windowClass))
    {
        MessageBoxA(0, "Window registration failed", "Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    u32 windowWidth  = engineState->width;
    u32 windowHeight = engineState->height;

    u32 windowStyle   = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX |
                        WS_THICKFRAME;
    u32 windowExStyle = WS_EX_APPWINDOW;

    // Obtain the size of the border.
    RECT borderRect = {0, 0, 0, 0};
    AdjustWindowRectEx(&borderRect, windowStyle, 0, windowExStyle);

    // Grow by the size of the OS border.
    windowWidth  += borderRect.right  - borderRect.left;
    windowHeight += borderRect.bottom - borderRect.top;

    platformState->hwnd = CreateWindowEx(
        windowExStyle, windowClass.lpszClassName, applicationName, windowStyle,
        CW_USEDEFAULT, CW_USEDEFAULT, windowWidth, windowHeight,
        0, 0, platformState->hinstance, 0
    );

    if (!platformState->hwnd)
    {
        MessageBoxA(0, "Window creation failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);

        PXFATAL("Window creation failed!");
        return 0;
    }

    SetWindowLongPtr(platformState->hwnd, 0, (LONG_PTR)(engineState));

    ShowWindow(platformState->hwnd, platformState->nCmdShow);

    return 1;
}

PXAPI inline void
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

PXAPI void *
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

PXAPI void
PlatformFreeMemory(void *memory, b8 aligned)
{
    if (memory)
    {
        VirtualFree(memory, 0, MEM_RELEASE);
    }
}

PXAPI f64
PlatformGetAbsoluteTime(platform_state *platformState)
{
    LARGE_INTEGER nowTime;
    QueryPerformanceCounter(&nowTime);
    return (f64)(nowTime.QuadPart - platformState->clockFrequency);
}

PXAPI void
PlatformDebugOutput(char *message, log_level level)
{
#if _DEBUG
    OutputDebugStringA((LPCSTR)message);

    b8 isError = level <= LOG_LEVEL_ERROR;

    DWORD colors[LOG_LEVEL_COUNT];
    colors[LOG_LEVEL_FATAL] = FOREGROUND_INTENSITY | BACKGROUND_RED | FOREGROUND_RED |
        FOREGROUND_GREEN | FOREGROUND_BLUE;
    colors[LOG_LEVEL_ERROR] = FOREGROUND_INTENSITY | FOREGROUND_RED;
    colors[LOG_LEVEL_WARN]  = FOREGROUND_RED | FOREGROUND_GREEN;
    colors[LOG_LEVEL_INFO]  = FOREGROUND_INTENSITY | FOREGROUND_BLUE;
    colors[LOG_LEVEL_DEBUG] = FOREGROUND_INTENSITY | FOREGROUND_GREEN;
    colors[LOG_LEVEL_TRACE] = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;

    LPDWORD numberWritten = 0;
    HANDLE hConsole = GetStdHandle(isError ? STD_OUTPUT_HANDLE : STD_ERROR_HANDLE);
    SetConsoleTextAttribute(hConsole, colors[level]);
    WriteConsoleA(hConsole, (LPCSTR)message, PXDEBUGStrLen(message), numberWritten, 0);
#endif // !_DEBUG
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

PXAPI b8
PlatformPumpMessages()
{
    MSG msg;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
    return 1;
#if 0
    MSG msg;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) > 0)
    {
        switch (msg.message)
        {
            case WM_QUIT:
            {
                return 0;
            }
            
            case WM_KEYDOWN:
            case WM_KEYUP:
            case WM_SYSKEYDOWN: //alt + keydown 
            case WM_SYSKEYUP: //alt + keyup
            {
                if (msg.wParam == VK_ESCAPE)
                {
                    PostQuitMessage(0);
                    return 0;
                }
            } break;
        }
    }
#endif

    return 0;
}
