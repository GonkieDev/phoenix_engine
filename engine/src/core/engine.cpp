#include "engine.hpp"

#include "logger.cpp"
#include "../memory/memory_arenas.cpp"

PXAPI b8 
PhoenixInit(engine_state *engineState)
{
    InitLogging();

    game_config gameConfig = {};
    b8 gameInitResult = GameInit(&gameConfig);
    if (!gameInitResult)
        return 0;

    engineState->width  = gameConfig.width;
    engineState->height = gameConfig.height;
    engineState->gameState = gameConfig.gameState;

    if (!PlatformInit(gameConfig.gameName, engineState))
    {
        PXFATAL("Platform failed to create!");
        return 0;
    }

    return 1;
}

PXAPI b8
PhoenixRun(engine_state *engineState)
{
    PXFATAL("omg %d", 2);
    PXERROR("omg %d", 5);
    PXWARN("omg %d", 23);
    PXINFO("omg %d", 64);
    PXDEBUG("test test test");
    PXTRACE("test test test");

    while (PlatformPumpMessages())
    {
        GameUpdate(0.f, engineState->gameState);
        GameRender(0.f, engineState->gameState);
    }

    GameEnd();

    return 1;
}
