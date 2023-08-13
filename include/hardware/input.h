#pragma once


#include "hardware/adc.h"
#include "hardware/joystick.h"
#include "fixed.h"

#define BUTTON_A_PIN 0
#define BUTTON_B_PIN 1
#define BUTTON_C_PIN 2

#define JOYSTICK_X_PIN 26
#define JOYSTICK_Y_PIN 27
#define JOYSTICK_PRESS_PIN 13

#define JOYSTICK_AXIS_DEADZONE 0.1fp
#define JOYSTICK_SAMPLE_SMOOTHING 3

namespace Input {
    enum Buttons {
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
        // fixed joystickAxis[2];
        fixed joystickAxisSamples[2][JOYSTICK_SAMPLE_SMOOTHING];
        int joystickAxisSampleIndex = 0;
        fixed joystickAxisTotal[2];
        fixed joystickAxis[2];
    };

    void Init(){
        // gpio_function(BUTTON_A_PIN, GPIO_FUNC_SIO);
        // gpio_function(BUTTON_B_PIN, GPIO_FUNC_SIO);
        // gpio_function(BUTTON_C_PIN, GPIO_FUNC_SIO);

        // gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
        // gpio_set_dir(BUTTON_B_PIN, GPIO_IN);
        // gpio_set_dir(BUTTON_C_PIN, GPIO_IN);

        gpio_set_function(JOYSTICK_PRESS_PIN, GPIO_FUNC_SIO);
        gpio_set_dir(JOYSTICK_PRESS_PIN, GPIO_IN);
        // set pullup resistor
        gpio_pull_up(JOYSTICK_PRESS_PIN);

        adc_init();
        adc_gpio_init(JOYSTICK_X_PIN);
        adc_gpio_init(JOYSTICK_Y_PIN);
    };

    void Poll(){
        bool buttonA = false; //(gpio_get(BUTTON_A_PIN) != 0);
        bool buttonB = false; //(gpio_get(BUTTON_B_PIN) != 0);
        bool buttonC = false; //(gpio_get(BUTTON_C_PIN) != 0);
        bool buttonStick = (gpio_get(JOYSTICK_PRESS_PIN) == 0);

        buttonsPressed[Buttons::A] = (buttonA && !buttonsDown[Buttons::A]);
        buttonsPressed[Buttons::B] = (buttonB && !buttonsDown[Buttons::B]);
        buttonsPressed[Buttons::C] = (buttonC && !buttonsDown[Buttons::C]);
        buttonsPressed[Buttons::Stick] = (buttonStick && !buttonsDown[Buttons::Stick]);

        buttonsDown[Buttons::A] = buttonA;
        buttonsDown[Buttons::B] = buttonB;
        buttonsDown[Buttons::C] = buttonC;
        buttonsDown[Buttons::Stick] = buttonStick;

        adc_select_input(0);
        fixed rawX = fixed((int)adc_read() - 2048)/2048;
        adc_select_input(1);
        fixed rawY = fixed((int)adc_read() - 2048)/2048;

        joystickAxisTotal[0] -= joystickAxisSamples[0][joystickAxisSampleIndex];
        joystickAxisTotal[1] -= joystickAxisSamples[1][joystickAxisSampleIndex];

        joystickAxisSamples[0][joystickAxisSampleIndex] = rawX;
        joystickAxisSamples[1][joystickAxisSampleIndex] = rawY;

        joystickAxisSampleIndex = (joystickAxisSampleIndex + 1) % JOYSTICK_SAMPLE_SMOOTHING;

        joystickAxisTotal[0] += rawX;
        joystickAxisTotal[1] += rawY;

        joystickAxis[0] = joystickAxisTotal[0] / JOYSTICK_SAMPLE_SMOOTHING;
        joystickAxis[1] = joystickAxisTotal[1] / JOYSTICK_SAMPLE_SMOOTHING;
        
        printf("X: %f, Y: %f, Stick: %d\n", (float)joystickAxis[Axis::X], (float)joystickAxis[Axis::Y], buttonStick);
    };

    bool IsButtonDown(Buttons button){
        return buttonsDown[button];
    };

    bool IsButtonPressed(Buttons button){
        return buttonsPressed[button];
    };

    fixed GetAxis(Axis axis){
        return abs(
            joystickAxis[axis]) < JOYSTICK_AXIS_DEADZONE ?
            0fp :
            (joystickAxis[axis] - JOYSTICK_AXIS_DEADZONE) / (1.0fp - JOYSTICK_AXIS_DEADZONE);
    };
};