#include "../core/engine.hpp"

// Includes for unity build
#include "../core/engine.cpp"

#include <vulkan/vulkan.h>

#include <windows.h>
#include <windowsx.h> // for GET_X_LPARAM, GET_Y_LPARAM
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

    PhoenixShutdown(&engineState);

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
    LARGE_INTEGER t;
    QueryPerformanceCounter(&t);
    return (f64)(t.QuadPart * platformState->clockFrequency);
}

PXAPI void
PlatformSleep(f64 secondsToSleep)
{
    HANDLE timer = CreateWaitableTimer(0, 1, 0);
    if(!timer) return;

    LARGE_INTEGER li;
    li.QuadPart = -NANOSECONDS((f32)secondsToSleep)/100; // timer given in 100ns seconds
    if(!SetWaitableTimer(timer, &li, 0, 0, 0, FALSE))
    {
        CloseHandle(timer);
        return;
    }
    // Start and wait for timer
    WaitForSingleObject(timer, INFINITE);

    CloseHandle(timer);
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

PXAPI inline void
win32ProcessKeyboardMessage(button_state *key, b32 isDown)
{
    /* PX_ASSERT_MSG(key->endedDown != isDown, "[INPUT] Button state didn't change but it's being processed as if it did"); */
    key->endedDown = isDown;
    ++key->halfTransitions;
}

PXAPI inline void
win32ProcessKeyInput(WPARAM wParam, LPARAM lParam, engine_state *engineState)
{
    u32 VKCode = (u32)(wParam);

    if (VKCode == VK_ESCAPE)
    {
        PostQuitMessage(0);
        engineState->isRunning = 0;
        return;
    }

    engine_input *input = &engineState->input;

    // https://learn.microsoft.com/en-us/windows/win32/inputdev/wm-keydown
    b32 wasDown = ((lParam & (1 << 30)) != 0);
    b32 isDown  = ((lParam & (1 << 31)) == 0);
    if (wasDown != isDown)
    {
        switch (VKCode)
        {
            case 'Q':
            win32ProcessKeyboardMessage(&input->Q, isDown); break;
            case 'W':
            win32ProcessKeyboardMessage(&input->W, isDown); break;
            case 'E':
            win32ProcessKeyboardMessage(&input->E, isDown); break;
            case 'R':
            win32ProcessKeyboardMessage(&input->R, isDown); break;
            case 'T':
            win32ProcessKeyboardMessage(&input->T, isDown); break;
            case 'Y':
            win32ProcessKeyboardMessage(&input->Y, isDown); break;
            case 'U':
            win32ProcessKeyboardMessage(&input->U, isDown); break;
            case 'I':
            win32ProcessKeyboardMessage(&input->I, isDown); break;
            case 'O':
            win32ProcessKeyboardMessage(&input->O, isDown); break;
            case 'P':
            win32ProcessKeyboardMessage(&input->P, isDown); break;
            case 'A':
            win32ProcessKeyboardMessage(&input->A, isDown); break;
            case 'S':
            win32ProcessKeyboardMessage(&input->S, isDown); break;
            case 'D':
            win32ProcessKeyboardMessage(&input->D, isDown); break;
            case 'F':
            win32ProcessKeyboardMessage(&input->F, isDown); break;
            case 'G':
            win32ProcessKeyboardMessage(&input->G, isDown); break;
            case 'H':
            win32ProcessKeyboardMessage(&input->H, isDown); break;
            case 'J':
            win32ProcessKeyboardMessage(&input->J, isDown); break;
            case 'K':
            win32ProcessKeyboardMessage(&input->K, isDown); break;
            case 'L':
            win32ProcessKeyboardMessage(&input->L, isDown); break;
            case 'Z':
            win32ProcessKeyboardMessage(&input->Z, isDown); break;
            case 'X':
            win32ProcessKeyboardMessage(&input->X, isDown); break;
            case 'C':
            win32ProcessKeyboardMessage(&input->C, isDown); break;
            case 'V':
            win32ProcessKeyboardMessage(&input->V, isDown); break;
            case 'N':
            win32ProcessKeyboardMessage(&input->N, isDown); break;
            case VK_SPACE:
            win32ProcessKeyboardMessage(&input->spacebar, isDown); break;
            case VK_OEM_4:
            win32ProcessKeyboardMessage(&input->leftBracket, isDown); break;
            case VK_OEM_6:
            win32ProcessKeyboardMessage(&input->rightBracket, isDown); break;

            // Mouse keys
            case VK_LBUTTON:
            win32ProcessKeyboardMessage(&input->mouseButtons[MOUSE_LEFT_BUTTON], isDown); break;
            case VK_RBUTTON:
            win32ProcessKeyboardMessage(&input->mouseButtons[MOUSE_RIGHT_BUTTON], isDown); break;
            case VK_MBUTTON:
            win32ProcessKeyboardMessage(&input->mouseButtons[MOUSE_MIDDLE_BUTTON], isDown); break;
            case VK_XBUTTON1:
            win32ProcessKeyboardMessage(&input->mouseButtons[MOUSE_SIDE_FORWARD_BUTTON], isDown); break;
            case VK_XBUTTON2:
            win32ProcessKeyboardMessage(&input->mouseButtons[MOUSE_SIDE_BACKWARD_BUTTON], isDown); break;
            
            default:
            PXWARN("[INPUT] %c(%d) key without mapped input being pressed", VKCode, VKCode);
        }
    }
}

PXAPI inline void
win32ProcessMouseMovementInput(i32 x, i32 y, engine_input *input)
{
    input->mouseX = x;
    input->mouseY = y;
}

PXAPI inline void
win32ProcessMouseWheelInput(i32 zDelta, engine_input *input)
{
    input->mouseDZ = zDelta;
}

PXAPI LRESULT CALLBACK 
win32WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    engine_state *engineState = (engine_state *)GetWindowLongPtr(hwnd, 0);

    switch (uMsg)
    {
        // NOTE: erasing will be handled by application to prevent flicker
        /* case WM_ERASEBKGND: return 1; */

        case WM_CLOSE:
        {
            // TODO: fire event to quit application
            engineState->isRunning = 0;
        } break;
        case WM_DESTROY:
        {
            engineState->isRunning = 0;
            PostQuitMessage(0);
        } break;

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
            win32ProcessKeyInput(wParam, lParam, engineState);
        } return 0;

        case WM_MOUSEMOVE:
        {
            i32 xMousePos = GET_X_LPARAM(lParam);
            i32 yMousePos = GET_Y_LPARAM(lParam);
            win32ProcessMouseMovementInput(xMousePos, yMousePos, &engineState->input);
        } return 0;
        case WM_MOUSEWHEEL:
        {
            i32 zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
            if (!zDelta) break;

            zDelta = (zDelta < 0) ? -1 : 1;
            win32ProcessMouseWheelInput(zDelta, &engineState->input);

        } return 0;
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
}
