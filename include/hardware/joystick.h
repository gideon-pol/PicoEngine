#pragma once

#include "hardware/gpio.h"

class Joystick {
public:
    Joystick(int gpioAxisX, int gpioAxisY, int gpioPress){
        this->gpioAxisX = gpioAxisX;
        this->gpioAxisY = gpioAxisY;
        this->gpioPress = gpioPress;
    }

    void Init(){
        gpio_set_function(gpioAxisX, GPIO_FUNC_SIO);
        gpio_set_function(gpioAxisY, GPIO_FUNC_SIO);
        gpio_set_function(gpioPress, GPIO_FUNC_SIO);

        gpio_set_dir(gpioAxisX, GPIO_IN);
        gpio_set_dir(gpioAxisY, GPIO_IN);
        gpio_set_dir(gpioPress, GPIO_IN);
    }

    int GetAxisX(){
        return gpio_get(gpioAxisX);
    }

    int GetAxisY(){
        return gpio_get(gpioAxisY);
    }

    bool IsStickPressed(){
        return (gpio_get(gpioPress) != 0);
    }
private:
    int gpioAxisX, gpioAxisY, gpioPress;
};