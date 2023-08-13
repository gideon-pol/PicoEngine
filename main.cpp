#include "pico/stdlib.h"
#include "pico/time.h"
#include "pico/multicore.h"

#include "common.h"

#include "mathematics/matrix.h"
#include "mathematics/vector.h"

#include "hardware/st7789.h"
#include "hardware/input.h"

#include "rendering/renderer.h"
#include "tests/rendering_tests.h"


// void core1_entry() {
    
// }

int main() {
    stdio_init_all();

    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);
    
    // multicore_launch_core1(core1_entry);
    sleep_ms(5000);

#ifdef ENABLE_OVERCLOCK
    vreg_set_voltage(VREG_VOLTAGE_1_20);
    set_sys_clock_khz(250000, true);
#endif

    ST7789::Init();
    ST7789::SetBrightness(65535);
    Input::Init();
    Renderer::Init(45, 0.1, 100);

    Color c = Color(32, 64, 128, 255);
    printf("Color %u %u %u %u\n", c.r, c.g, c.b, c.a);
    uint16_t c16 = c.ToColor16();
    c = Color(c.ToColor16());
    printf("Color16 %x\n", c16);
    printf("Color %u %u %u %u\n", c.r, c.g, c.b, c.a);

    picoCubeTest();

    while (true) {
        gpio_put(25, 1);
        sleep_ms(1000);
        gpio_put(25, 0);
        sleep_ms(1000);
    }

    return 0;
}

