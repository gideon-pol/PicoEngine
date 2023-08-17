#include "hardware/input.h"
#include "hardware/adc.h"

#ifdef PLATFORM_PICO
void Input::Init(){
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

void Input::Poll(){
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

bool Input::IsButtonDown(Buttons button){
    return buttonsDown[button];
};

bool Input::IsButtonPressed(Buttons button){
    return buttonsPressed[button];
};

fixed Input::GetAxis(Axis axis){
    return abs(
        joystickAxis[axis]) < JOYSTICK_AXIS_DEADZONE ?
        0fp :
        (joystickAxis[axis] - JOYSTICK_AXIS_DEADZONE) / (1.0fp - JOYSTICK_AXIS_DEADZONE);
};
#endif