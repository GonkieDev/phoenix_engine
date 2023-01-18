#pragma once

#include "../defines.hpp"
// Core
#include "asserts.h"
#include "logger.hpp"
#include "input.hpp"

struct platform_state;
struct game_state; // NOTE: declared by user

struct engine_state
{
    b8 isRunning;
    b8 isSuspended;
    platform_state *platformState;
    u16 width;
    u16 height;
    f64 lastTime;
    engine_input input;

    game_state *gameState;
};

// Memory
#include "../memory/memory_arenas.hpp"
// Platform
#include "../platform/platform.hpp"

PXAPI b8 PhoenixInit(engine_state *appState);
PXAPI b8 PhoenixRun(engine_state *appState);

struct game_config
{
    u16 width;
    u16 height;
    wchar_t *gameName;
    game_state *gameState;
};
b8 GameInit(game_config *gameConfig);

void GameUpdate(f32 deltaTime, game_state *gameState);
void GameRender(f32 deltaTime, game_state *gameState);
void GameOnResize(u32 height, u32 width, game_state *gameState);

b8 GameEnd();
