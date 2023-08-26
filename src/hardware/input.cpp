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
    
    // printf("X: %f, Y: %f, Stick: %d\n", (float)joystickAxis[Axis::X], (float)joystickAxis[Axis::Y], buttonStick);
};
#endif

#ifdef PLATFORM_NATIVE
void Input::Init(){};

// This is separated from the HandleEvent function, as key presses are separated into multiple events
// that are given from further up the engine. We don't want to reset presses for every event.
void Input::Poll(){
    buttonsPressed[Button::A] = false;
    buttonsPressed[Button::B] = false;
    buttonsPressed[Button::C] = false;
    buttonsPressed[Button::Stick] = false;

    joystickAxis[Axis::X] = 0fp;
    joystickAxis[Axis::Y] = 0fp;
};

void Input::HandleEvent(SDL_Event* event){
    switch(event->type){
        case SDL_KEYDOWN:
            switch(event->key.keysym.sym){
                case SDLK_a:
                    buttonsPressed[Button::A] = true;
                    buttonsDown[Button::A] = true;
                    break;
                case SDLK_s:
                    buttonsPressed[Button::B] = true;
                    buttonsDown[Button::B] = true;
                    break;
                case SDLK_d:
                    buttonsPressed[Button::C] = true;
                    buttonsDown[Button::C] = true;
                    break;
                case SDLK_SPACE:
                    buttonsPressed[Button::Stick] = true;
                    buttonsDown[Button::Stick] = true;
                    break;
                case SDLK_RIGHT:
                    joystickAxis[Axis::X] = 1fp;
                    break;
                case SDLK_LEFT:
                    joystickAxis[Axis::X] = -1fp;
                    break;
                case SDLK_UP:
                    joystickAxis[Axis::Y] = -1fp;
                    break;
                case SDLK_DOWN:
                    joystickAxis[Axis::Y] = 1fp;
                    break;
            }
            break;
        case SDL_KEYUP:
            switch(event->key.keysym.sym){
                case SDLK_a:
                    buttonsDown[Button::A] = false;
                    break;
                case SDLK_s:
                    buttonsDown[Button::B] = false;
                    break;
                case SDLK_d:
                    buttonsDown[Button::C] = false;
                    break;
                case SDLK_SPACE:
                    buttonsDown[Button::Stick] = false;
                    break;
                case SDLK_RIGHT:
                    joystickAxis[Axis::X] = 0fp;
                    break;
                case SDLK_LEFT:
                    joystickAxis[Axis::X] = 0fp;
                    break;
                case SDLK_UP:
                    joystickAxis[Axis::Y] = 0fp;
                    break;
                case SDLK_DOWN:
                    joystickAxis[Axis::Y] = 0fp;
                    break;
            }
            break;
        case SDL_MOUSEMOTION:
            joystickAxis[Axis::X] = fixed(event->motion.xrel) / fixed(SCREEN_WIDTH) * 512;
            joystickAxis[Axis::Y] = fixed(event->motion.yrel) / fixed(SCREEN_HEIGHT) * 512;
            break;
    }
};
#endif

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