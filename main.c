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

    // renderer_draw_rect(5, 300, 1, 40);
    // renderer_draw_rect(10, 300, 1, 40);

    // renderer_draw_rect(5, 410, 1, 40);
    // renderer_draw_rect(10, 410, 1, 40);
    // renderer_draw_rect(5, 450, 6, 40);

    // renderer_draw_image(10, 100, test_width, test_height, test_bits);

    printf("Test\r\n");
    printf("Clock speed %d\r\n", clock_get_hz(clk_sys));

    struct repeating_timer timer;
    add_repeating_timer_ms(25, repeating_timer_callback, NULL, &timer);

    while(1) {
        if (redraw) {
            xpos++;
            renderer_begin_drawing();
            renderer_draw_rect(xpos, 0, 50, 200);
            renderer_end_drawing();
            redraw = false;
        }
    }

    return 0;
}
