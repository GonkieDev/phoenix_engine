#pragma once


struct platform_state;
struct game_state; // NOTE: declared by user

#include "../defines.hpp"
// Core
#include "asserts.h"
#include "logger.hpp"
#include "input.hpp"
#include "clock.hpp"

struct engine_state
{
    b8 isRunning;
    b8 isSuspended;

    u16 width = 720;
    u16 height = 720;
    u16 targetRefreshRate; // NOTE: 0 means that it's unlimitted

    clock clock;
    f64 lastTime;
    engine_input input;

    platform_state *platformState;
    game_state *gameState;
};

// Memory
#include "../memory/memory_arenas.hpp"
// Platform
#include "../platform/platform.hpp"

struct game_config
{
    u16 width;
    u16 height;
    u16 targetRefreshRate;
    wchar_t *gameName = L"Phoenix Engine";
    game_state *gameState;
};
b8 GameInit(game_config *gameConfig);

void GameUpdate(f32 deltaTime, engine_input input, game_state *gameState);
void GameRender(f32 deltaTime, game_state *gameState);
void GameOnResize(u32 height, u32 width, game_state *gameState);

void GameEnd(game_state *gameState);
