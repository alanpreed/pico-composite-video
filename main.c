#include <stdio.h>
#include "pico/stdlib.h"

#include "hardware/dma.h"
#include "hardware/timer.h"
#include "hardware/clocks.h"

#include "renderer.h"
#include "connections.h"

#include "test.xbm"

bool redraw = false;
uint32_t xpos = 0;

bool repeating_timer_callback(struct repeating_timer* t) {
    // printf("Repeat at %lld\n",time_us_64());
    redraw = true;


    return true;
}

int main()
{
    // Defaults: UART 0, TX pin 0, RX pin 1, baud rate 115200
    stdio_init_all();
    gpio_set_function(SERIAL_RX_PIN, GPIO_FUNC_UART);
    gpio_set_function(SERIAL_TX_PIN, GPIO_FUNC_UART);

    puts("Hello, world!");
    renderer_init();

    printf("Test\r\n");
    printf("Clock speed %d\r\n", clock_get_hz(clk_sys));

    struct repeating_timer timer;
    add_repeating_timer_ms(50, repeating_timer_callback, NULL, &timer);

    while(1) {
        if (redraw) {
            uint32_t time_before = time_us_32();
            renderer_begin_drawing();
            renderer_draw_rect(xpos, 200, 10, 100);

            renderer_draw_rect(768 - (xpos % 768), 400, 30, 100);
            renderer_draw_character(100, 300, 2, 'b');
            renderer_draw_string(100, 200, 2, "Hello screen", 12);
            renderer_draw_string(100, 250, 4, "Hello screen", 12);
            // renderer_draw_image(xpos, 100, test_width, test_height, test_bits);
            renderer_end_drawing();
            uint32_t time_after = time_us_32();
            redraw = false;
            xpos++;
            printf("drawing time %d \r\n", time_after - time_before);
            while(1);
        }
    }

    return 0;
}
