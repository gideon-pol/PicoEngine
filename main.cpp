#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/time.h"
#include "pico/multicore.h"
#include "mathematics/matrix.h"
#include "mathematics/vector.h"
#include "tests/maths_tests.h"
#include "tests/rendering_tests.h"

#define DEBUG_TEST

// void core1_entry() {
    
// }

int main() {
    stdio_init_all();

    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);
    
    // multicore_launch_core1(core1_entry);

#ifdef DEBUG_TEST
    sleep_ms(5000);
    // printf("Running math unit tests\n");
    // maths_test();

    printf("Running rendering unit tests\n");
    absolute_time_t time = get_absolute_time();
    drawCubeTest();

    uint32_t diff = to_ms_since_boot(get_absolute_time()) - to_ms_since_boot(time);
    printf("Drawing took: %u ms\n", diff);

#endif
    vec3f v1 = vec3f::up;

    while (true) {
        absolute_time_t time = get_absolute_time();
        gpio_put(25, 1);

        sleep_ms(1000);

        printf("Hello, world!\n");
        gpio_put(25, 0);
        sleep_ms(1000);
        uint32_t diff = to_ms_since_boot(get_absolute_time()) - to_ms_since_boot(time);
        printf("Printing and blinking took: %u\n", diff);
    }

    return 0;
}

