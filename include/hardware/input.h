#ifndef INPUT_H
#define INPUT_H


#include "fixed.h"

#ifdef PLATFORM_NATIVE
#include <SDL2/SDL.h>
#endif

#define BUTTON_A_PIN 0
#define BUTTON_B_PIN 1
#define BUTTON_C_PIN 2

#define JOYSTICK_X_PIN 26
#define JOYSTICK_Y_PIN 27
#define JOYSTICK_PRESS_PIN 13

#define JOYSTICK_AXIS_DEADZONE 0.1fp
#define JOYSTICK_SAMPLE_SMOOTHING 3

namespace Input {
    enum Button {
        A = 0,
        B = 1,
        C = 2,
        Stick = 3,
    };

    enum Axis {
        X = 0,
        Y = 1,
    };

    namespace {
        bool buttonsDown[4];
        bool buttonsPressed[4];
        fixed joystickAxisSamples[2][JOYSTICK_SAMPLE_SMOOTHING];
        int joystickAxisSampleIndex = 0;
        fixed joystickAxisTotal[2];
        fixed joystickAxis[2];
    };

    void Init();
    #ifdef PLATFORM_NATIVE
    void HandleEvent(SDL_Event* event);
    #endif
    void Poll();
    bool GetButtonDown(Button button);
    bool GetButtonPress(Button button);
    fixed GetAxis(Axis axis);
};

#endif