#include "hardware/input.h"

#ifdef PLATFORM_PICO
#include "hardware/adc.h"

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

    buttonsPressed[Button::A] = (buttonA && !buttonsDown[Button::A]);
    buttonsPressed[Button::B] = (buttonB && !buttonsDown[Button::B]);
    buttonsPressed[Button::C] = (buttonC && !buttonsDown[Button::C]);
    buttonsPressed[Button::Stick] = (buttonStick && !buttonsDown[Button::Stick]);

    buttonsDown[Button::A] = buttonA;
    buttonsDown[Button::B] = buttonB;
    buttonsDown[Button::C] = buttonC;
    buttonsDown[Button::Stick] = buttonStick;

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

bool Input::GetButtonDown(Button button){
    return buttonsDown[button];
};

bool Input::GetButtonPress(Button button){
    return buttonsPressed[button];
};

fixed Input::GetAxis(Axis axis){
    return abs(
        joystickAxis[axis]) < JOYSTICK_AXIS_DEADZONE ?
        0fp :
        (joystickAxis[axis] - JOYSTICK_AXIS_DEADZONE) / (1.0fp - JOYSTICK_AXIS_DEADZONE);
};
#endif

#ifdef PLATFORM_NATIVE
void Input::Init(){};
void Input::Poll(){};
bool Input::GetButtonDown(Button button){return false;};
bool Input::GetButtonPress(Button button){return false;};
fixed Input::GetAxis(Axis axis){return 0fp;};
#endif
