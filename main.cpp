#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/time.h"
#include "pico/multicore.h"
#include "mathematics/matrix.h"
#include "mathematics/vector.h"
#include "tests/maths_tests.h"
#include "hardware/st7789.h"

#include "rendering/renderer.h"
#include "tests/rendering_tests.h"

#define DEBUG_TEST

// void core1_entry() {
    
// }

int main() {
    stdio_init_all();

    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);
    
    // multicore_launch_core1(core1_entry);
    sleep_ms(5000);

    ST7789::Init();

    Color16 fb[120*120];
    Renderer::Init(45, 0.1, 100);

    ST7789::SetBrightness(65535);

#ifdef DEBUG_TEST
    printf("Running rendering unit tests\n");

    picoCubeTest();
#endif
    while (true) {
        gpio_put(25, 1);
        sleep_ms(1000);
        gpio_put(25, 0);
        sleep_ms(1000);
    }

    return 0;
}

