#include <core/engine.hpp>

struct game_state
{
    u32 test;
};


b8 GameInit(game_config *gameConfig)
{
    wchar_t *gameName = L"Test Bed";

    gameConfig->gameName = gameName;
    gameConfig->width  = 1000;
    gameConfig->height = 720;

    /*
     Do stuff like:
     Init memory according to how user wants it
     Load assets
     Configure pipeline
     Hot reloads

     */

    return 1;
}

void GameUpdate(f32 deltaTime, game_state *gameState)
{
}

void GameRender(f32 deltaTime, game_state *gameState)
{
}

void GameOnResize(u32 height, u32 width, game_state *gameState)
{
}

b8 GameEnd()
{
    return 1;
}
