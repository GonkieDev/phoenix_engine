#pragma once

#include "../defines.hpp"

#ifndef PX_MAX_GAMEPADS
#define PX_MAX_GAMEPADS 4
#endif

#define INPUT_LEFT_STICK_DEADZONE 7849
#define INPUT_RIGHT_STICK_DEADZONE 8689
#define INPUT_TRIGGER_THRESHHOLD 30

struct button_state
{
    i32 halfTransitions; 
    b32 endedDown;    
};

enum MOUSE_BUTTONS
{
    MOUSE_LEFT_BUTTON,
    MOUSE_RIGHT_BUTTON,
    MOUSE_MIDDLE_BUTTON,
    MOUSE_SIDE_FORWARD_BUTTON,
    MOUSE_SIDE_BACKWARD_BUTTON,

    MOUSE_BUTTON_COUNT
};

struct gamepad_input
{
    b16 dPadUp;
    b16 dPadDown;
    b16 dPadLeft;
    b16 dPadRight;

    b16 leftThumb;
    b16 rightThumb;

    b16 leftShoulder;
    b16 rightShoulder;

    b16 A, B, X, Y;

    b16 start;
    b16 back;

    u8 leftTrigger;
    u8 rightTrigger;

    // NOTE(gonk): Negative values are left or down
    i16 leftStickX;
    i16 leftStickY;
    i16 rightStickX;
    i16 rightStickY;
};

struct engine_input
{
    gamepad_input padInput[PX_MAX_GAMEPADS];

    //Keyboard
    //TODO: add more keys
    union 
    {
        button_state keys[36];
        struct 
        {
            button_state spacebar;
            button_state A;
            button_state B;
            button_state C;
            button_state D;
            button_state E;
            button_state F;
            button_state G;
            button_state H;
            button_state I;
            button_state J;
            button_state K;
            button_state L;
            button_state M;
            button_state N;
            button_state O;
            button_state P;
            button_state Q;
            button_state R;
            button_state S;
            button_state T;
            button_state U;
            button_state V;
            button_state W;
            button_state X;
            button_state Y;
            button_state Z;
            button_state n1;
            button_state n2;
            button_state n3;
            button_state n4;
            button_state n5;
            button_state n6;
            button_state n7;
            button_state n8;
            button_state n9;
            button_state n0;
            
            button_state leftBracket;
            button_state rightBracket;
        };
    };
    
    //Mouse
    button_state mouseButtons[MOUSE_BUTTON_COUNT];
    i32 mouseX, mouseY;
    f32 mouseDX, mouseDY;
    f32 mouseDZ; // NOTE: [-1, 1]
};
