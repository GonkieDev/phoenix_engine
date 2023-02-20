#pragma once

struct platform_state;
struct game_state; // NOTE: declared by user

#include <defines.hpp>
// Core
#include <core/asserts.h>
#include <core/logger.hpp>
#include <core/input.hpp>
#include <core/clock.hpp>
#include <core/pxstring.hpp>
#include <renderer/renderer_frontend.hpp>
// Math
#include <math/math_types.hpp>
#include <math/pxmath.hpp>
// Memory
#include <memory/memory.hpp>
// Platform
#include <platform/platform.hpp>

struct engine_state
{
    b8 isRunning;
    b8 isSuspended;

    // Settings - TODO: do we want to keep these here?
    u16 width;
    u16 height;
    u16 targetRefreshRate;

    clock clock;
    f64 lastTime;
    engine_input input;

    mem_arena permArena;  // NOTE: mem arena that only gets reset at the end of the program's lifetime
    mem_arena frameArena; // NOTE: mem arena that gets reset every frame (also after initialisation,
                          // thus it can be used for temporary memory needed for initialisation)

    platform_state *platformState;
    game_state *gameState;

    renderer_backend rendererBackend;
};

//
// User API
//

/*
 *  General game/app configurations
 *      - NOTE: only change things you want, the rest will bet set to default
 */
struct game_config
{
    u16 width  = 720;
    u16 height =  720;
    u16 targetRefreshRate = 60;  // NOTE: 0 means that it's unlimitted
    wchar_t *gameName = L"Phoenix Engine";
    game_state *gameState = 0;

    s64 frameArenaSize = KIBIBYTES(100);
    s64 permArenaSize  = GIBIBYTES(2);
};
b8 GameInit(game_config *gameConfig);

void GameUpdate(f32 deltaTime, engine_input input, game_state *gameState);
void GameRender(f32 deltaTime, game_state *gameState);
void GameOnResize(u32 height, u32 width, game_state *gameState);

void GameEnd(game_state *gameState);
