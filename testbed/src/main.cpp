#include <core/engine.hpp>

void GameMain(platform_state_ptr platformState)
{
    wchar_t *gameName = L"Test Bed";
    PhoenixInit(720, 720, gameName, platformState);
}
