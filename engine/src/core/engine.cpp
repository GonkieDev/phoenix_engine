#include "engine.hpp"

#include "logger.cpp"
#include "clock.cpp"
#include <memory/memory_arenas.cpp>
#include <renderer/renderer_frontend.cpp>


PXAPI b8 
PhoenixInit(engine_state *engineState)
{
    InitLogging();

    //
    // Configs
    //

    game_config gameConfig = {};
    b8 gameInitResult = GameInit(&gameConfig);
    if (!gameInitResult)
        return 0;

    // Only set configs if provided, else keep default values
    if (gameConfig.width) engineState->width = gameConfig.width;
    if (gameConfig.height) engineState->height = gameConfig.height;
    if (gameConfig.targetRefreshRate) engineState->targetRefreshRate = gameConfig.targetRefreshRate;

    engineState->gameState = gameConfig.gameState;

    //
    // Platform
    //

    if (!PlatformInit(gameConfig.gameName, engineState))
    {
        PXFATAL("Platform failed to create!");
        return 0;
    }

    //
    // Renderer
    //

    if (!RendererInit(gameConfig.gameName, engineState))
    {
        PXFATAL("Renderer failed to create!");
        return 0;
    }

    //
    // Input
    //

    engineState->input = {};

    engineState->isRunning = 1;
    return 1;
}

PXAPI b8
PhoenixRun(engine_state *engineState)
{
    ClockStart(&engineState->clock, engineState->platformState);
    ClockUpdate(&engineState->clock, engineState->platformState);
    engineState->lastTime = engineState->clock.elapsed;
    f64 runningTime = 0;
    f64 targetFrameSeconds = 0;
    if (engineState->targetRefreshRate)
        targetFrameSeconds = 1.0 / (f64)engineState->targetRefreshRate;
    u8 frameCount = 0;

    while (engineState->isRunning)
    {
        PlatformPumpMessages();

        if (!engineState->isSuspended)
        {
            ClockUpdate(&engineState->clock, engineState->platformState);
            f64 deltaTime = (engineState->clock.elapsed - engineState->lastTime);
            f64 frameStartTime = PlatformGetAbsoluteTime(engineState->platformState);

            GameUpdate((f32)deltaTime, engineState->input, engineState->gameState);
            GameRender((f32)deltaTime, engineState->gameState);

            // NOTE: temporary
            render_packet packet;
            packet.deltaTime = deltaTime;
            RendererDrawFrame(&packet);

            // Reset input for next frame
            engineState->input = {};

            // Get time taken to run this function
            f64 frameEndTime = PlatformGetAbsoluteTime(engineState->platformState);
            f64 frameElapsedTime = frameEndTime - frameStartTime;
            runningTime += frameElapsedTime;
            f64 remainingFrameTime = targetFrameSeconds - frameElapsedTime;
            if (remainingFrameTime > 0)
            {
                PlatformSleep(remainingFrameTime);
                frameCount++;
            }
            engineState->lastTime = engineState->clock.elapsed;
        }

    }

    engineState->isRunning = 0;

    return 1;
}

PXAPI void
PhoenixShutdown(engine_state *engineState)
{
    GameEnd(engineState->gameState);

    RendererShutdown(engineState);
}
