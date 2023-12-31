#include "pico/stdlib.h"
#include "pico/time.h"
#include "pico/multicore.h"

#include "common.h"
#include "time.hpp"

#include "mathematics.h"

#include "hardware/st7789.h"
#include "hardware/input.h"

#include "rendering/renderer.h"
#include "tests/rendering_tests.h"

extern void game_init();
extern void game_update();
extern void game_render();

int main() {
    stdio_init_all();

    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);
    
    // multicore_launch_core1(core1_entry);

#ifdef ENABLE_OVERCLOCK
    vreg_set_voltage(VREG_VOLTAGE_1_20);
    set_sys_clock_khz(250000, true);
#endif

    ST7789::Init();
    ST7789::SetBrightness(65535);

    Time::Init();
    Input::Init();
    Renderer::Init();

    game_init();

    while (true) {
        Time::Tick();
        Input::Poll();

        game_update();

        while(ST7789::IsFlipping());

        Renderer::Prepare();
        game_render();
        ST7789::Flip((Color565*)&Renderer::FrameBuffer);
    }

    return 0;
}

