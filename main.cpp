#include "pico/stdlib.h"
#include "pico/time.h"
#include "pico/multicore.h"

#include "common.h"
#include "time.hpp"

#include "mathematics.h"

#include "hardware/st7789.h"
#include "hardware/input.h"

#include "rendering/renderer.h"
#include "rendering/postprocessing.h"
#include "tests/rendering_tests.h"

#include <iostream>

extern void game_init();
extern void game_update();
extern void game_mesh_render();
extern void game_ui_render();

void core1() {
    while (true) {
        multicore_fifo_pop_blocking();
        while (Renderer::Render());
        Renderer::Finish();
    }
}

int main() {
    stdio_init_all();

    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);
    
    multicore_launch_core1(core1);

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

        multicore_fifo_push_blocking(0);

        game_mesh_render();

        while(Renderer::Render());
        Renderer::Finish();

        PostProcessing::Apply((Color565*)&Renderer::FrameBuffer, vec2i16(120, 120));

        game_ui_render();

        ST7789::Flip((Color565*)&Renderer::FrameBuffer);
    }

    return 0;
}

