#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/time.h"
#include "pico/multicore.h"
#include "mathematics/matrix.h"
#include "mathematics/vector.h"
#include "tests/maths_tests.h"

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
    printf("Running math unit tests\n");
    maths_test();


#endif
    vec3f v1 = vec3f::up;

    while (true) {
        absolute_time_t time = get_absolute_time();
        gpio_put(25, 1);

        mat4 m4 = mat4::rotate(90.0/180*PI, vec3f::up);

        printf("Rotational matrix had %d rows and %d columns\n", m4.rows(), m4.columns());
        for (int i = 0; i < m4.rows(); i++) {
            for(int j = 0; j < m4.columns(); j++){
                printf("%f ", m4[i][j]);
            }
            printf("\n");
        }

        // printf("Vec: %f %f %f %f length: %f\n", v5.x, v5.y, v5.z, v5.w, v5.magnitude());


        sleep_ms(1000);

        printf("Hello, world!\n");
        gpio_put(25, 0);
        sleep_ms(1000);
        uint32_t diff = to_ms_since_boot(get_absolute_time()) - to_ms_since_boot(time);
        printf("Printing and blinking took: %u\n", diff);
    }

    return 0;
}

