#include <core/engine.hpp>

// Core
#include <core/logger.cpp>
#include <core/clock.cpp>
#include <core/pxstring.cpp>
// Math
#include <math/pxmath.cpp>
// Memory
#include <memory/memory.cpp>
#include <renderer/renderer_frontend.cpp>

PXAPI b8 PhoenixInitMemory(s64 permArenaSize, s64 frameArenaSize, engine_state *engineState);

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

    engineState->width  = gameConfig.width;
    engineState->height = gameConfig.height;
    engineState->targetRefreshRate = gameConfig.targetRefreshRate;
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
    // Memory
    //

    if (!PhoenixInitMemory(gameConfig.permArenaSize, gameConfig.frameArenaSize, engineState))
    {
        PXFATAL("Failed to allocate memory required!");
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

    // Reset frame arena
    PXMemoryArenaClear(&engineState->frameArena);

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
            RendererDrawFrame(&packet, engineState);

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

            // Reset frame arena
            PXMemoryArenaClear(&engineState->frameArena);
        }
    }

    engineState->isRunning = 0;

    return 1;
}

PXAPI inline void
PhoenixShutdown(engine_state *engineState)
{
    GameEnd(engineState->gameState);

    RendererShutdown(engineState);

    ShutdownLogging();
}

PXAPI inline void
PhoenixOnWindowResize(u32 width, u32 height, engine_state *engineState)
{
    // Window minimisation
    if (width == 0 || height == 0)
    {
        PXINFO("Suspended.");
        engineState->isSuspended = 1;
        return;
    }

    engineState->isSuspended = 0;

    if (width == engineState->width && height == engineState->height)
    {
        return;
    }

    engineState->width = width;
    engineState->height = height;

    RendererOnResized(width, height, engineState);

    GameOnResize(engineState->width, engineState->height, engineState->gameState);
}

PXAPI inline b8
PhoenixInitMemory(s64 permArenaSize, s64 frameArenaSize, engine_state *engineState)
{
    PX_ASSERT_MSG(permArenaSize, "A permanent arena size is required!");
    PX_ASSERT_MSG(frameArenaSize, "A frame arena size is required!");

    s64 totalSize = permArenaSize + frameArenaSize;

    void *base = PlatformAllocateMemory(totalSize, 0);

    if (!base)
    {
        PXFATAL("Unable to allocate main memory!");
        return 0;
    }

    engineState->permArena = {};
    engineState->permArena.buf    = (u8*)base;
    engineState->permArena.offset = 0;
    engineState->permArena.size   = permArenaSize;

    engineState->frameArena = {};
    engineState->frameArena.buf    = (u8*)(engineState->permArena.buf + engineState->permArena.size);
    engineState->frameArena.offset = 0; 
    engineState->frameArena.size   = frameArenaSize; 

    return 1;
}
